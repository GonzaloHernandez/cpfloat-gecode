#include <cpfloat/cpfloat.hh>
#include <cstdlib>

using Gecode::Home;
using Gecode::Space;
using Gecode::Choice;
using Gecode::ExecStatus;
using Gecode::ES_FAILED;
using Gecode::ES_OK;

namespace MPG { namespace CPFloat { namespace Branch {

/**
 * \brief Simple, tuple based brancher
 *
 * Simple brancher on a relation variable that selects a tuple and decides
 * to include and exclude it.
 */
class NoneMin : public Gecode::Brancher {
protected:
  CPFloatView x_;
  /// Simple, tuple-based relation choice
  class SimpleChoice : public Gecode::Choice {
  public:
    /// Tuple to branch on
    BoundType med_;
    /// Constructor
    SimpleChoice(const NoneMin& b, BoundType med)
      : Choice(b,2), med_(med) {}
    /// Returns the size of the object
    virtual size_t size(void) const {
      return sizeof(*this);
    }
    virtual void archive(Gecode::Archive&) const {
      assert(false);
    }
  };
public:
  /// Constructor for a brancher on variable \a x
  NoneMin(Home home, CPFloatView x)
    : Brancher(home), x_(x) {}
  /// Brancher posting
  static void post(Home home, CPFloatView x) {
    (void) new (home) NoneMin(home,x);
  }
  /// Constructor for clonning
  NoneMin(Space& home, bool share, NoneMin& b)
    : Brancher(home,share,b) {
    x_.update(home,share,b.x_);
  }
  /// Brancher copying
  virtual Brancher* copy(Space& home, bool share) {
    return new (home) NoneMin(home,share,*this);
  }
  /// Brancher disposal
  virtual size_t dispose(Space& home) {
    (void) Brancher::dispose(home);
    return sizeof(*this);
  }
  /// Returns the status of the brancher
  virtual bool status(const Space&) const {
    if (!x_.assigned())
      return true;
    return false;
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(Space&) {
    /// \todo Here we ar finding the median of the interval. The problem is that
    /// during the commit we are using geq and leq constraints. This in my opinion
    /// will lead to the exploration of the same subtree twice. Is that right?
    BoundType m = x_.median();
    return new SimpleChoice(*this,m);
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(const Space&, Gecode::Archive&) {
    assert(false);
    return (SimpleChoice*)NULL;
  }
  /// Commit choice
  virtual ExecStatus commit(Space& home, const Choice& c, unsigned int a) {
    const SimpleChoice& ch = static_cast<const SimpleChoice&>(c);
    BoundType r = ch.med_;
    if (a == 0)
      return Gecode::me_failed(x_.geq(home,r)) ? ES_FAILED : ES_OK;
    else
      return Gecode::me_failed(x_.leq(home,r)) ? ES_FAILED : ES_OK;
  }
};

/**
 * \brief First Fail, array based brancher
 *
 * Custom brancher on a relation variable that select a most left variable
 * with the minimum size.
 */
class FirstFail : public Gecode::Brancher {
protected:
  Gecode::ViewArray<CPFloat::CPFloatView> x_;
  mutable int start;
   /// Simple, tuple-based relation choice
  class PosVal : public Choice {
  public:
    int pos_; BoundType val_;
    PosVal(const FirstFail& b, int p, BoundType v)
      : Choice(b,2), pos_(p), val_(v) {}

    virtual size_t size(void) const {
      return sizeof(*this);
    }
    virtual void archive(Gecode::Archive& e) const {
      Choice::archive(e);
      e << pos_;
    }
  };
public:
  /// Constructor for a brancher on variable array \a x
  FirstFail(Home home, Gecode::ViewArray<CPFloat::CPFloatView>& x0)
    : Brancher(home), x_(x0) {}
  /// Brancher posting
  static void post(Home home, Gecode::ViewArray<CPFloat::CPFloatView>& x) {
    (void) new (home) FirstFail(home,x);
  }
  /// Constructor for clonning
  FirstFail(Space& home, bool share, FirstFail& b)
    : Brancher(home,share,b) {
    x_.update(home,share,b.x_);
  }
  /// Brancher copying
  virtual Brancher* copy(Space& home, bool share) {
    return new (home) FirstFail(home,share,*this);
  }
  /// Brancher disposal
  virtual size_t dispose(Space& home) {
    (void) Brancher::dispose(home);
    return sizeof(*this);
  }
  /// Returns the status of the brancher
  virtual bool status(const Space& /*home*/) const {
    for (int i=0; i<x_.size(); i++)
      if (!x_[i].assigned()) {
        start = i;
        return true;
      }
    return false;
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(Space& /*home*/) {
    int p = start;
    BoundType s = x_[p].lub()-x_[p].glb();
    for (int i=start+1; i<x_.size(); i++)
      if (!x_[i].assigned() && (x_[p].lub()-x_[p].glb())<s) {
        p = i; s = x_[p].lub()-x_[p].glb();
      }
    return new PosVal(*this,p,x_[p].median());
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(const Space&, Gecode::Archive& e) {
    int pos;
    e >> pos;
    return new PosVal(*this, pos, x_[pos].median());
  }
  /// Commit choice
  virtual ExecStatus commit(Space& home, const Choice& c, unsigned int a) {
    const PosVal& pv = static_cast<const PosVal&>(c);
    int pos=pv.pos_;
    BoundType val=pv.val_;
    if (a == 0)
      return Gecode::me_failed(x_[pos].geq(home,val)) ? ES_FAILED : ES_OK;
    else
      return Gecode::me_failed(x_[pos].leq(home,val)) ? ES_FAILED : ES_OK;
  }
};

/**
 * \brief Naive, array based brancher
 *
 * Custom brancher on a relation variable that selects a variable in order
 * of left to right
 */
class Naive : public Gecode::Brancher {
protected:
  Gecode::ViewArray<CPFloat::CPFloatView> x_;
  mutable int start;
   /// Simple, tuple-based relation choice
  class PosVal : public Choice {
  public:
    int pos_; BoundType val_;
    PosVal(const Naive& b, int p, BoundType v)
      : Choice(b,2), pos_(p), val_(v) {}

    virtual size_t size(void) const {
      return sizeof(*this);
    }
    virtual void archive(Gecode::Archive&) const {
      assert(false);
    }
  };
public:
  /// Constructor for a brancher on variable array \a x
  Naive(Home home, Gecode::ViewArray<CPFloat::CPFloatView>& x0)
    : Brancher(home), x_(x0) {}
  /// Brancher posting
  static void post(Home home, Gecode::ViewArray<CPFloat::CPFloatView>& x) {
    (void) new (home) Naive(home,x);
  }
  /// Constructor for clonning
  Naive(Space& home, bool share, Naive& b)
    : Brancher(home,share,b) {
    x_.update(home,share,b.x_);
  }
  /// Brancher copying
  virtual Brancher* copy(Space& home, bool share) {
    return new (home) Naive(home,share,*this);
  }
  /// Brancher disposal
  virtual size_t dispose(Space& home) {
    (void) Brancher::dispose(home);
    return sizeof(*this);
  }
  /// Returns the status of the brancher
  virtual bool status(const Space& /*home*/) const {
    for (int i=0; i<x_.size(); i++)
      if (!x_[i].assigned()) {
        start = i;
        return true;
      }
    return false;
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(Space& /*home*/) {
    for (int i=0; i<x_.size(); i++)
      if (!x_[i].assigned()) {
        return new PosVal(*this,i,x_[i].median());
      }
    return NULL;
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(const Space&, Gecode::Archive&) {
    assert(false);
    return (PosVal*)NULL;
  }
  /// Commit choice
  virtual ExecStatus commit(Space& home, const Choice& c, unsigned int a) {
    const PosVal& pv = static_cast<const PosVal&>(c);
    int pos=pv.pos_;
    BoundType val=pv.val_;
    if (a == 0)
      return Gecode::me_failed(x_[pos].geq(home,val)) ? ES_FAILED : ES_OK;
    else
      return Gecode::me_failed(x_[pos].leq(home,val)) ? ES_FAILED : ES_OK;
  }
};

/**
 * \brief Rand, array based brancher
 *
 * Custom brancher on a relation variable that selects a random variable
 */
class Rand : public Gecode::Brancher {
protected:
  Gecode::ViewArray<CPFloat::CPFloatView> x_;
  mutable int start;
   /// Simple, tuple-based relation choice
  class PosVal : public Choice {
  public:
    int pos_; BoundType val_;
    PosVal(const Rand& b, int p, BoundType v)
      : Choice(b,2), pos_(p), val_(v) {}

    virtual size_t size(void) const {
      return sizeof(*this);
    }
    virtual void archive(Gecode::Archive&) const {
      assert(false);
    }
  };
public:
  /// Constructor for a brancher on variable array \a x
  Rand(Home home, Gecode::ViewArray<CPFloat::CPFloatView>& x0)
    : Brancher(home), x_(x0) {}
  /// Brancher posting
  static void post(Home home, Gecode::ViewArray<CPFloat::CPFloatView>& x) {
    (void) new (home) Rand(home,x);
  }
  /// Constructor for clonning
  Rand(Space& home, bool share, Rand& b)
    : Brancher(home,share,b) {
    x_.update(home,share,b.x_);
  }
  /// Brancher copying
  virtual Brancher* copy(Space& home, bool share) {
    return new (home) Rand(home,share,*this);
  }
  /// Brancher disposal
  virtual size_t dispose(Space& home) {
    (void) Brancher::dispose(home);
    return sizeof(*this);
  }
  /// Returns the status of the brancher
  virtual bool status(const Space& /*home*/) const {
    for (int i=0; i<x_.size(); i++)
      if (!x_[i].assigned()) {
        start = i;
        return true;
      }
    return false;
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(Space& home) {
    int unassignedCount=0;
    for (int i=0; i<x_.size(); i++) {
      if (!x_[i].assigned()) unassignedCount++;
    }
    Gecode::ViewArray<CPFloat::CPFloatView> unassignedVars(home,unassignedCount);
    for (int i=0,r=0; i<x_.size(); i++) {
      if (!x_[i].assigned()) {
        unassignedVars[r++] = x_[i];
      }
    }
    int pos = rand() % unassignedCount;

    return new PosVal(*this,pos,unassignedVars[pos].median());
  }
  /// Creates a choice by selecting a tuple from the unknown of the variable
  virtual Choice* choice(const Space&, Gecode::Archive&) {
    assert(false);
    return (PosVal*)NULL;
  }
  /// Commit choice
  virtual ExecStatus commit(Space& home, const Choice& c, unsigned int a) {
    const PosVal& pv = static_cast<const PosVal&>(c);
    int pos=pv.pos_;
    BoundType val=pv.val_;
    if (a == 0)
      return Gecode::me_failed(x_[pos].geq(home,val)) ? ES_FAILED : ES_OK;
    else
      return Gecode::me_failed(x_[pos].leq(home,val)) ? ES_FAILED : ES_OK;
  }
};

}}

void branch(Home home, CPFloatVar x) {
  using namespace CPFloat::Branch;
  if (home.failed()) return;
  NoneMin::post(home,x);
}

void firstfail(Home home, const CPFloatVarArgs& x) {
  using namespace CPFloat::Branch;
  if (home.failed()) return;
  Gecode::ViewArray<CPFloat::CPFloatView> y(home,x);
  FirstFail::post(home,y);
}

void naive(Home home, const CPFloatVarArgs& x) {
  using namespace CPFloat::Branch;
  if (home.failed()) return;
  Gecode::ViewArray<CPFloat::CPFloatView> y(home,x);
  Naive::post(home,y);
}

void randselection(Home home, const CPFloatVarArgs& x) {
  using namespace CPFloat::Branch;
  if (home.failed()) return;
  Gecode::ViewArray<CPFloat::CPFloatView> y(home,x);
  Rand::post(home,y);
}

}

