#ifndef __CPFLOAT_CPFLOAT_HH__
#define __CPFLOAT_CPFLOAT_HH__

#include <iostream>
#include <sstream>
#include <cfloat>

#include <gecode/kernel.hh>
#include <boost/numeric/interval.hpp>
#include <smath/smath.h>


using Gecode::Advisor;
using Gecode::ConstView;
using Gecode::Delta;
using Gecode::DerivedView;
using Gecode::Exception;
using Gecode::ModEvent;
using Gecode::ModEventDelta;
using Gecode::PropCond;
using Gecode::Propagator;
using Gecode::Space;
using Gecode::VarArgArray;
using Gecode::VarArray;
using Gecode::VarImpVar;
using Gecode::VarImpView;

// exceptions
namespace MPG { namespace CPFloat {

class OutOfLimits : public Exception {
public:
  OutOfLimits(const char* l)
    : Exception(l,"Number out of limits") {}
};

class VariableEmptyDomain : public Exception {
public:
  VariableEmptyDomain(const char* l)
    : Exception(l,"Attempt to create variable with empty domain") {}
};

}}

// variable implementation
namespace MPG { namespace CPFloat {

/// Underlying type to represent an interval.
typedef double BoundType;
/// Underlying type to represent an interval.
typedef boost::numeric::interval<BoundType> Interval;

// limits
namespace Limits {
//const int max = (INT_MAX / 2) - 1;
//const int min = -max;
const double max = (DBL_MAX / 2) -1;
const double min = -max;
}
// delta for advisors
class CPFloatDelta : public Delta {
private:
  BoundType l, u;
public:
  CPFloatDelta(BoundType min, BoundType max) : l(min), u(max) {}
  BoundType min(void) const {
    return l;
  }
  BoundType max(void) const {
    return u;
  }
};

/**
 * \brief Float domain.
 *
 * A float domain is approximated by means of a greatest lower
 * bound \a glb and a least upper bound \a lub.
 */
class CPFloatVarImp : public CPFloatVarImpBase {
protected:
  /// Implementation for the interval
  Interval  impl_;
  BoundType presicion_;
public:
  /// \name Constructors and disposers
  //@{
  /// Constructor for a variable created between lower bound and upper bound.
  CPFloatVarImp(Space& home, BoundType l, BoundType u, BoundType p=0.000000001)
    : CPFloatVarImpBase(home), impl_(l,u), presicion_(p) {}
  /// Resources disposal
  void dispose(Space&) {
  }
  //@}
  /// \name Bound access
  //@{
  /// Returns a float representing the greatest lower bound of the variable
  BoundType glb(void) const {
    return impl_.lower();
  }
  /// Returns a float representing the least upper bound of the variable
  BoundType lub(void) const {
    return impl_.upper();
  }
  /// Returns the median of the interval
  BoundType median(void) const {
    boost::numeric::interval_lib::rounded_math<BoundType> rnd;
    return rnd.div_down( rnd.add_down(impl_.upper(),impl_.lower()) , 2.0);
  }

  //@}
  /// \name Pruning operations
  //@{
  /**
   * \brief Prune the variable by doing: \f$ glb = glb \cup r \f$
   *
   */
  ModEvent geq(Space& home, BoundType f) {
    if (assigned()) return ME_CPFLOAT_VAL;
    if ( boost::numeric::interval_lib::cerge(impl_, f) ) {
      return ME_CPFLOAT_NONE;
    }
    if ( !boost::numeric::interval_lib::posge(impl_, f) ) {
      return ME_CPFLOAT_FAILED;
    }

    impl_ = boost::numeric::max(f, impl_);
    CPFloatDelta d(impl_.lower(),f);
    return notify(home, assigned() ? ME_CPFLOAT_VAL : ME_CPFLOAT_MIN, d);
  }
  /**
   * \brief Prune the variable by doing: \f$ lub = lub \setminus r \f$
   *
   */
  ModEvent leq(Space& home, BoundType f) {
    if (assigned()) return ME_CPFLOAT_VAL;
    if ( boost::numeric::interval_lib::cerle(impl_, f) ) {
      return ME_CPFLOAT_NONE;
    }
    if ( !boost::numeric::interval_lib::posle(impl_, f) ) {
      return ME_CPFLOAT_FAILED;
    }
    impl_ = boost::numeric::min(impl_, f);
    CPFloatDelta d(f,impl_.upper());
    return notify(home, assigned() ? ME_CPFLOAT_VAL : ME_CPFLOAT_MAX, d);
  }
  //@}
  /// \name Domain tests
  //@{
  /// Tests for assignment \f$ glb = lub \f$
  bool assigned(void) const {
    //std::cout << " *** assigned() *** " << impl_.lower() << " " << impl_.upper() << " " << presicion_ << " Dif: " << impl_.upper()-impl_.lower() << std::endl;
    return (impl_.upper()-impl_.lower()<presicion_);
  }
  //@}
  /// \name Subscriptions handling
  //@{
  // subscriptions
  void subscribe(Space& home, Propagator& p, PropCond pc, bool schedule=true) {
    CPFloatVarImpBase::subscribe(home,p,pc,assigned(),schedule);
  }
  void subscribe(Space& home, Advisor& a) {
    CPFloatVarImpBase::subscribe(home,a,assigned());
  }
  void cancel(Space& home, Propagator& p, PropCond pc) {
    CPFloatVarImpBase::cancel(home,p,pc,assigned());
  }
  void cancel(Space& home, Advisor& a) {
    CPFloatVarImpBase::cancel(home,a,assigned());
  }
  //@}
  /// \name Copying
  //@{
  CPFloatVarImp(Space& home, bool share, CPFloatVarImp& y)
    : CPFloatVarImpBase(home,share,y), impl_(y.impl_), presicion_(y.presicion_) {}

  CPFloatVarImp* copy(Space& home, bool share) {
    if (copied())
      return static_cast<CPFloatVarImp*>(forward());
    else
      return new (home) CPFloatVarImp(home,share,*this);
  }
  //@}
  // delta information
  static BoundType min(const Delta& d) {
    return static_cast<const CPFloatDelta&>(d).min();
  }
  static BoundType max(const Delta& d) {
    return static_cast<const CPFloatDelta&>(d).max();
  }
};

}}

// variable
namespace MPG {
/**
 * \brief Float Var.
 *
 * A float var is a var of constraint programming user for represent of a
 * continuous domain.  This Float Var is implemented for CPFloatVarImp.
 *
 */
class CPFloatVar : public VarImpVar<CPFloat::CPFloatVarImp> {
protected:
  using VarImpVar<CPFloat::CPFloatVarImp>::x;
public:
  /// \name Constructors
  //@{
  /// Constructor for a empty variable
  CPFloatVar(void) {}
  /// Constructor for a variable created as copy of another Float Var
  CPFloatVar(const CPFloatVar& y)
    : VarImpVar<CPFloat::CPFloatVarImp>(y.varimp()) {}
  /// Constructor for a variable created using a Var Implementation
  CPFloatVar(CPFloat::CPFloatVarImp *y)
    : VarImpVar<CPFloat::CPFloatVarImp>(y) {}
  /// Constructor for a variable created between lower bound and upper bound.
  CPFloatVar(Space& home, CPFloat::BoundType l, CPFloat::BoundType u, CPFloat::BoundType precision=0.000000001)
    : VarImpVar<CPFloat::CPFloatVarImp>
      (new (home) CPFloat::CPFloatVarImp(home,l,u,precision)) {
    if (l > u)
      throw CPFloat::VariableEmptyDomain("CPFloatVar::CPFloatVar");
  }
  //@}
  /// \name Domain information
  //@{
  /**
   * \brief Greatest lower bound access.
   *
   * Returns a float (copy) that represents the lower bound of the variable.
   */
  CPFloat::BoundType glb(void) const {
    return x->glb();
  }
  /**
   * \brief Least upper bound access.
   *
   * Returns a float (copy) that represents the upper bound of the variable.
   */
  CPFloat::BoundType lub(void) const {
    return x->lub();
  }
  //@}
};

template<class Char, class Traits>
std::basic_ostream<Char,Traits>&
operator <<(std::basic_ostream<Char,Traits>& os, const CPFloatVar& x) {
  std::basic_ostringstream<Char,Traits> s;
  s.copyfmt(os); s.width(0);

  using namespace CPFloat;
  s << "{" << x.glb() << " , " << x.lub() << "}";

  if (x.assigned()) s << " * ";

  return os << s.str();
}

}

// array traits
namespace MPG {
class CPFloatVarArgs; class CPFloatVarArray;
}

namespace Gecode {

template<>
class ArrayTraits<Gecode::VarArray<MPG::CPFloatVar> > {
public:
  typedef MPG::CPFloatVarArray  StorageType;
  typedef MPG::CPFloatVar       ValueType;
  typedef MPG::CPFloatVarArgs   ArgsType;
};
template<>
class ArrayTraits<MPG::CPFloatVarArray> {
public:
  typedef MPG::CPFloatVarArray  StorageType;
  typedef MPG::CPFloatVar       ValueType;
  typedef MPG::CPFloatVarArgs   ArgsType;
};
template<>
class ArrayTraits<Gecode::VarArgArray<MPG::CPFloatVar> > {
public:
  typedef MPG::CPFloatVarArgs   StorageType;
  typedef MPG::CPFloatVar       ValueType;
  typedef MPG::CPFloatVarArgs   ArgsType;
};
template<>
class ArrayTraits<MPG::CPFloatVarArgs> {
public:
  typedef MPG::CPFloatVarArgs  StorageType;
  typedef MPG::CPFloatVar      ValueType;
  typedef MPG::CPFloatVarArgs  ArgsType;
};

}

// variable arrays
namespace MPG {
/// Passing float variables
class CPFloatVarArgs : public VarArgArray<CPFloatVar> {
public:
  CPFloatVarArgs(void) {}
  explicit CPFloatVarArgs(int n) : VarArgArray<CPFloatVar>(n) {}
  CPFloatVarArgs(const CPFloatVarArgs& a) : VarArgArray<CPFloatVar>(a) {}
  CPFloatVarArgs(const VarArray<CPFloatVar>& a) : VarArgArray<CPFloatVar>(a) {}
  CPFloatVarArgs(Space& home, int n, CPFloat::BoundType l, CPFloat::BoundType u)
    : VarArgArray<CPFloatVar>(n) {
    for (int i=0; i<n; i++)
      (*this)[i] = CPFloatVar(home,l,u);
  }
};

class CPFloatVarArray : public VarArray<CPFloatVar> {
public:
  CPFloatVarArray(void) {}
  CPFloatVarArray(const CPFloatVarArray& a)
    : VarArray<CPFloatVar>(a) {}
  CPFloatVarArray(Space& home, int n, CPFloat::BoundType l, CPFloat::BoundType u, CPFloat::BoundType precision=0.000000001)
    : VarArray<CPFloatVar>(home,n) {
    for (int i=0; i<n; i++)
      (*this)[i] = CPFloatVar(home,l,u,precision);
  }
};

}

// float view
namespace MPG { namespace CPFloat {
/// Float view for float variables
class CPFloatView : public VarImpView<CPFloatVar> {
protected:
  using VarImpView<CPFloatVar>::x;
public:
  CPFloatView(void) {}
  CPFloatView(const CPFloatVar& y)
    : VarImpView<CPFloatVar>(y.varimp()) {
    }
  CPFloatView(CPFloatVarImp* y)
    : VarImpView<CPFloatVar>(y) {}
  // access operations
  BoundType glb(void) const {
    return x->glb();
  }
  BoundType lub(void) const {
    return x->lub();
  }
  BoundType median(void) const {
    return x->median();
  }

  // modification operations
  ModEvent geq(Space& home, BoundType f) {
    return x->geq(home,f);
  }
  ModEvent leq(Space& home, BoundType f) {
    return x->leq(home,f);
  }
  // delta information
  int min(const Delta& d) const {
    return CPFloatVarImp::min(d);
  }
  int max(const Delta& d) const {
    return CPFloatVarImp::max(d);
  }
  bool operator==(CPFloatView v) {
    return (x == v.x);
  }
};

template<class Char, class Traits>
std::basic_ostream<Char,Traits>&
operator<<(std::basic_ostream<Char,Traits>& os, const CPFloatView& x) {
  std::basic_ostringstream<Char,Traits> s;
  s.copyfmt(os); s.width(0);
  s << "{" << x.glb() << " " << x.lub() << "}";

  if (x.assigned()) s << "*";

  return os << s.str();
}

}
}

#include <cpfloat/prop-advanced/expression.hh>

namespace MPG {

  void branch(Gecode::Home home, CPFloatVar x);
  void firstfail(Gecode::Home home, const CPFloatVarArgs& x);
  void naive(Gecode::Home home, const CPFloatVarArgs& x);
  void randselection(Gecode::Home home, const CPFloatVarArgs& x);
  void sin(Gecode::Space& home, CPFloatVar x, CPFloatVar y);
  void cos(Gecode::Space& home, CPFloatVar x, CPFloatVar y);
  void tan(Gecode::Space& home, CPFloatVar x, CPFloatVar y);
  void asin(Gecode::Space& home, CPFloatVar x, CPFloatVar y);
  void acos(Gecode::Space& home, CPFloatVar x, CPFloatVar y);
  void atan(Gecode::Space& home, CPFloatVar x, CPFloatVar y);
  void equality(Gecode::Space& home, CPFloatVar x, CPFloatVar y);
  void addition(Gecode::Space& home, CPFloatVar x, CPFloatVar y, CPFloatVar z);
  void subtraction(Gecode::Space& home, CPFloatVar x, CPFloatVar y, CPFloatVar z);
  void times(Gecode::Space& home, CPFloatVar x, CPFloatVar y, CPFloatVar z);
  void power(Gecode::Space& home, CPFloatVar x, int e, CPFloatVar y);
  
  void hc4(Gecode::Space& home, CPFloat::Constraint& cst);
  void k3b(Gecode::Space& home, CPFloat::Constraint& cst);
}

#endif
