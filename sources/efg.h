//
// FILE: efg.h -- Declaration of extensive form data type
//
// $Id$
//

#ifndef EFG_H
#define EFG_H

#include "gstring.h"
#include "gblock.h"
#include "glist.h"

#include "gpvector.h"


class EFOutcome;
class EFPlayer;
class Infoset;
class Node;
class Action;
class EFSupport;

class BaseEfg     {
  
private:
  // this is used to track memory leakage; #define MEMCHECK to use it
#ifdef MEMCHECK
  static int _NumObj;
#endif   // MEMCHECK  

friend class EfgFileReader;

protected:
    bool sortisets;
    gString title;
    gBlock<EFPlayer *> players;
    gBlock<EFOutcome *> outcomes;
    Node *root;
    EFPlayer *chance;

       //# PROTECTED CONSTRUCTORS -- FOR DERIVED CLASS USE ONLY
    BaseEfg(void);
    BaseEfg(const BaseEfg &);

    void CopySubtree(Node *, Node *, Node *);
    void MarkSubtree(Node *);
    void UnmarkSubtree(Node *);

    void DisplayTree(gOutput &, Node *) const;

    virtual Infoset *CreateInfoset(int n, EFPlayer *pl, int br) = 0;
    virtual Node *CreateNode(Node *parent) = 0;

    void SortInfosets(void);

  
// These are auxiliary functions used by the .efg file reader code
    Infoset *GetInfosetByIndex(EFPlayer *p, int index) const;
    Infoset *CreateInfosetByIndex(EFPlayer *p, int index, int br);
    EFOutcome *GetOutcomeByIndex(int index) const;
    virtual EFOutcome *CreateOutcomeByIndex(int index) = 0;
    void Reindex(void);
    
    virtual void DeleteLexicon(void) = 0;

    EFOutcome *NewOutcome(int index);

// These are used in identification of subgames
    void MarkTree(Node *, Node *);
    bool CheckTree(Node *, Node *);
    void MarkSubgame(Node *, Node *);

  public:
       //# DESTRUCTOR
    virtual ~BaseEfg();

       //# TITLE ACCESS AND MANIPULATION
    void SetTitle(const gString &s);
    const gString &GetTitle(void) const;

       //# WRITING DATA FILES
    void DisplayTree(gOutput &) const;

       //# DATA ACCESS -- GENERAL INFORMATION
    virtual DataType Type(void) const = 0;
    virtual bool IsConstSum(void) const = 0;

    Node *RootNode(void) const;
    bool IsSuccessor(const Node *n, const Node *from) const;
    bool IsPredecessor(const Node *n, const Node *of) const;

       //# DATA ACCESS -- PLAYERS
    int NumPlayers(void) const;

    EFPlayer *GetChance(void) const;
    virtual EFPlayer *NewPlayer(void) = 0;
    const gArray<EFPlayer *> &Players(void) const  { return players; }

       //# DATA ACCESS -- OUTCOMES
    int NumOutcomes(void) const;
    const gArray<EFOutcome *> &Outcomes(void) const  { return outcomes; }
    void DeleteOutcome(EFOutcome *c);  
 
       //# EDITING OPERATIONS
    Infoset *AppendNode(Node *n, EFPlayer *p, int br);
    Infoset *AppendNode(Node *n, Infoset *s);
    Node *DeleteNode(Node *n, Node *keep);
    Infoset *InsertNode(Node *n, EFPlayer *p, int br);
    Infoset *InsertNode(Node *n, Infoset *s);

    virtual Infoset *CreateInfoset(EFPlayer *pl, int br) = 0;
    bool DeleteEmptyInfoset(Infoset *);
    Infoset *JoinInfoset(Infoset *s, Node *n);
    Infoset *LeaveInfoset(Node *n);
    Infoset *SplitInfoset(Node *n);
    Infoset *MergeInfoset(Infoset *to, Infoset *from);

    Infoset *SwitchPlayer(Infoset *s, EFPlayer *p);

    Node *CopyTree(Node *src, Node *dest);
    Node *MoveTree(Node *src, Node *dest);
    Node *DeleteTree(Node *n);

    Action *InsertAction(Infoset *s);
    Action *InsertAction(Infoset *s, Action *at);
    Infoset *DeleteAction(Infoset *s, Action *a);

    void Reveal(Infoset *, const gArray<EFPlayer *> &);

    // Unmarks all subgames in the subtree rooted at n
    void UnmarkSubgames(Node *n);
    bool IsLegalSubgame(Node *n);

    // Mark all the (legal) subgames in the list
    void MarkSubgames(const gList<Node *> &list);
    bool DefineSubgame(Node *n);
    void RemoveSubgame(Node *n);


    int ProfileLength(void) const;
    gArray<int> NumInfosets(void) const;
    gPVector<int> NumActions(void) const;
    gPVector<int> NumMembers(void) const;
};

#include "behav.h"

template <class T> class EfgFile;
template <class T> class BehavProfile;


#ifndef EFG_ONLY
template <class T> class Nfg;
template <class T> class MixedProfile;
template <class T> class Lexicon;
#endif   // EFG_ONLY

#include "grblock.h"

template <class T> class Efg : public BaseEfg   {
#ifndef EFG_ONLY
  friend class BaseNfg;
  friend class Nfg<T>;
#endif   // EFG_ONLY
  friend class EfgFile<T>;
  private:
#ifndef EFG_ONLY
    Lexicon<T> *lexicon;
    Nfg<T> *afg;
#endif  EFG_ONLY
    gRectBlock<T> payoffs;

    Efg<T> &operator=(const Efg<T> &);

    void Payoff(Node *n, T, const gPVector<int> &, gVector<T> &) const;
    void Payoff(Node *n, T, const gArray<gArray<int> *> &, gVector<T> &) const;

    void InfosetProbs(Node *n, T, const gPVector<int> &, gPVector<T> &) const;

    Infoset *CreateInfoset(int n, EFPlayer *pl, int br);
    Node *CreateNode(Node *parent);
    EFOutcome *CreateOutcomeByIndex(int index);


    // this is for use with the copy constructor
    void CopySubtree(Node *, Node *);

    void DeleteLexicon(void);

    void WriteEfgFile(gOutput &, Node *) const;

  public:
	//# CONSTRUCTORS AND DESTRUCTOR
    Efg(void);
    Efg(const BaseEfg &);
    Efg(const Efg<T> &);
    Efg(const Efg<T> &, Node *);
    virtual ~Efg(); 

	//# DATA ACCESS -- GENERAL INFORMATION
    DataType Type(void) const;
    bool IsConstSum(void) const;
    T MinPayoff(int pl = 0) const;
    T MaxPayoff(int pl = 0) const;

    void WriteEfgFile(gOutput &f) const;


    EFPlayer *NewPlayer(void);

        //# DATA ACCESS -- OUTCOMES
    EFOutcome *NewOutcome(void);

    Infoset *CreateInfoset(EFPlayer *pl, int br);

    void SetPayoff(EFOutcome *, int pl, const T &value);
    T Payoff(EFOutcome *, int pl) const;
    gVector<T> Payoff(EFOutcome *) const;

    void SetChanceProb(Infoset *, int, const T &);
    T GetChanceProb(Infoset *, int) const;
    gArray<T> GetChanceProbs(Infoset *) const;

        //# COMPUTING VALUES OF PROFILES

    void Payoff(const gPVector<int> &profile, gVector<T> &payoff) const;
    void Payoff(const gArray<gArray<int> *> &profile, 
		gVector<T> &payoff) const;

    void InfosetProbs(const gPVector<int> &profile, gPVector<T> &prob) const;

#ifndef EFG_ONLY
    friend Nfg<T> *MakeReducedNfg(Efg<T> &, const EFSupport &);
    friend Nfg<T> *MakeAfg(Efg<T> &);
    friend void MixedToBehav(const Nfg<T> &N, const MixedProfile<T> &mp,
		             const Efg<T> &E, BehavProfile<T> &bp);
    friend void BehavToMixed(const Efg<T> &, const BehavProfile<T> &,
			     const Nfg<T> &, MixedProfile<T> &);
#endif   // EFG_ONLY 
   
    // defined in efgutils.cc
    friend void RandomEfg(Efg<T> &);

#ifndef EFG_ONLY
    // This function put in to facilitate error-detection in MixedToBehav[]
    friend Nfg<T> *AssociatedNfg(Efg<T> *E);
    friend Nfg<T> *AssociatedAfg(Efg<T> *E);
#endif   // EFG_ONLY
};

#include "efplayer.h"
#include "infoset.h"
#include "node.h"
#include "outcome.h"

// These functions are provided in readefg.y/readefg.cc
template <class T> int ReadEfgFile(gInput &, Efg<T> *&);
void EfgFileType(gInput &f, bool &valid, DataType &type);

#endif   // EFG_H


