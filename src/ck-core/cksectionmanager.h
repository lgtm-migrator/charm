#ifndef _CK_SECTION_MANAGER_
#define _CK_SECTION_MANAGER_
#include "cksec.decl.h"

#include "cksection.h"
#include <unordered_map>

class SectionMulticastMsg : public CkMcastBaseMsg, public CMessage_SectionMulticastMsg
{
public:
  ck::SectionID sid;
  int ep;
  CProxy_CkSectionManager secmgr;
  SectionMulticastMsg(ck::SectionID _sid, int _ep, CProxy_CkSectionManager _secmgr)
    : sid{_sid}, ep{_ep}, secmgr{_secmgr} {}
  void pup(PUP::er &p);
};


// placeholder, but I think this will be some base Proxy class
struct _SectionMember
{
  using collection_type = CkGroupID;
  using element_type = CkArrayIndex;
  CkGroupID collection;
  CkArrayIndex element;
  _SectionMember() {}
  _SectionMember(CkGroupID c, CkArrayIndex e)
    : collection{c}, element{e}
  {}

};

class _SectionInfo
{
public:
  template<class T>
  using LocalMemberContainer = std::vector<T>;
  using SectionMember = _SectionMember;
private:
  CkSectionInfo info;
  LocalMemberContainer<SectionMember> localElements;
  // children in the spanning tree, but I think we will (for now) defer to Ckmulticast for the spanning tree.
  std::vector<int> childPEs;

  void initializeInfo(SectionMember::collection_type collection)
  {
    // NOTE: Doesn't work for x-array sections
    if(localElements.empty())
      {
        this->info = CkSectionInfo(CkMyPe(), NULL, 0, collection);
      }
  }

public:
  using size_type = LocalMemberContainer<SectionMember>::size_type;
  _SectionInfo(size_type localElementSize)
    : localElements{localElementSize} {}
  _SectionInfo()
    : localElements{0} {}

  void addLocalMember(SectionMember::collection_type collection,
                      SectionMember::element_type element
                      )
  {
    initializeInfo(collection);
    localElements.emplace_back(collection, element);
  }
  template<typename InputIt>
  void addLocalMembers(SectionMember::collection_type collection,
                       InputIt begin, InputIt end
                       )
  {
    initializeInfo(collection);
    localElements.insert(localElements.end(), begin, end);
  }
  void addChildPE(int pe)
  {
    childPEs.push_back(pe);
  }

  const LocalMemberContainer<SectionMember>&
  getLocalMembers() {return localElements;}

};

class CkSectionManager : public CBase_CkSectionManager {
public:
  using SectionInfo = CkSectionID;
  using SectionMapType = std::unordered_map<ck::SectionID, SectionInfo>;
private:
  SectionMapType sections;
  CkMulticastMgr *mcastMgr;
  int lastCounter = 0;
  int myPe = -1;

  ck::SectionID createSectionID();

  template<class OutputIt>
  void createSectionIDs(OutputIt dest, int n)
  {
    for(int _ = 0; _ < n; ++_)
      {
        *dest = createSectionID();
        ++dest;
      }
  }

public:

  CkSectionManager()
    : myPe{CkMyPe()}
  {
    // Where is this created for regular sections? For each new section upon creation?
    CkGroupID mCastGrpId = CProxy_CkMulticastMgr::ckNew();
    this->mcastMgr = CProxy_CkMulticastMgr(mCastGrpId).ckLocalBranch();
  }
  // Create a single section containing the chares in the range
  // [begin, end), return a handle to it that can be referenced
  // returns SectionID, will be used to create CProxy_SectionXX
  // CProxy_SectionXX will have the SectionID as a member
  template<class SectionFn, class InputIt, class ElementProxy>
  ck::SectionID createSection(SectionFn fn, ElementProxy collection, InputIt begin, InputIt end)
  {
    ck::SectionID newSectionID = createSectionID();
    SectionInfo newSectionInfo{};
    newSectionInfo._cookie = CkSectionInfo{};
    auto aid = collection.ckGetArrayID();

    for(auto x = begin; x != end; x++)
      {
        if(fn(*x))
          {
            // can add a "getID" specialized for array/group members
            newSectionInfo.addMember(*x);
          }
      }

    // this needs to happen on each processor?
    this->mcastMgr->initDelegateMgr(newSectionInfo, aid);
    // this->mcastMgr->setSection(newSectionInfo._cookie, aid, newSectionInfo._elems.data(),
    //                            newSectionInfo.nElems()
    //                            );
    // // should move it, not copy
    // SectionCreationMsg *creationMsg = new SectionCreationMsg(newSectionID, newSectionInfo);
    // CkGetSectionInfo(newSectionInfo._cookie, creationMsg);
    // broadcast to all sectionmanagers this new section
    sections[newSectionID] = newSectionInfo;

    return newSectionID;
  }

  void initSectionLocal(SectionMulticastMsg *m);
  void contribute(ck::SectionID sid, int size, void *data,
                  CkReduction::reducerType reduction, CkCallback cb
                  );
  void multicast(ck::SectionID sid, int ep);

  // private method
  void doMulticast(ck::SectionID sid, int ep);
  // void send(ck::SectionID sid, int ep, void *m)
  // {

  //   this->mcastMgr->ArraySectionSend();
  // }

  // create std::distance(outputBegin, outputEnd) sections from the chares in the range
  // [begin, end).
  // writes the section IDs of the created sections to the range [outputBegin, outputEnd)
  template<class SectionFn, class InputIt, class OutputIt, class ElementProxy>
  void createSection(SectionFn fn, ElementProxy collection, InputIt begin, InputIt end, OutputIt outputBegin, OutputIt outputEnd)
  {
    // auto nSections = std::distance(outputEnd, outputBegin);
    // ck::SectionID ids[nSections];
    // createSectionIDs(ids, nSections);
    // int idIdx = 0;
    // auto aid = collection.ckGetArrayID();

    // for(auto x = outputBegin; x != outputEnd; ++x)
    //   {
    //     auto id = ids[idIdx];
    //     *x = id;
    //     idIdx++;

    //     sections[id] = _SectionInfo();
    //   }

    // for(const auto x = begin; x != end; ++x)
    //   {
    //     // assumes each chare is in exactly one section, but that may not be the case.
    //     // From here we could: specify that SectionFn can generate a sequence of section IDs
    //     // or it can return a vector of section ID
    //     auto sectionNum = fn(*x);
    //     auto sectionID = ids[sectionNum];
    //     sections[sectionID].addLocalMember(aid, *x);
    //   }

  }

  // combine sections in the range [begin, end) where
  // each item in the range is a proxy to a section that has already been created
  template<typename InputIt>
  ck::SectionID combineSections(InputIt begin,  InputIt end)
  {
    ck::SectionID newSectionID = createSectionID();
    // _SectionInfo newSectionInfo{};

    // for(auto x = begin; x != end; ++x)
    //   {
    //     const auto& members = *x.getLocalMembers();
    //     newSectionInfo.addLocalMembers(members.begin(), members.end());
    //   }
    // sections[newSectionID] = newSectionInfo;
    return newSectionID;
  }

};

#endif // _CK_SECTION_MANAGER_
