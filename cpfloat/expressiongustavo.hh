#ifndef __CPFLOAT_EXPRESSION_HH__
#define __CPFLOAT_EXPRESSION_HH__

#include <boost/numeric/interval.hpp>
#include <cpfloat/cpfloat.hh>

namespace MPG {
// Forward declaration of the variable class
class CPFloatVar;

namespace CPFloat {

enum ExprType {
  VAR,
  CONS,
  ADD,
  SUB,
  MUL,
  DIV,
  POW
};

enum RelType {
  EQUAL
};

class SymbolicExpression {
public:
  /// Output the expression to \a stdout
  virtual void print(void) const = 0;
  /// Computes the number of variables stored in the expression
  virtual int countVariables(void) const = 0;
  /// Adds the variables in the constraint to the array \a a starting from possition \a i
  virtual void addVariables(Gecode::ViewArray<CPFloatView>& a, int& i) = 0;
  /// Add all the views in the expression in \a v starting at possition \a i
  void addViews(Gecode::ViewArray<CPFloatView>& v, int& i) {
    addVariables(v,i);
  }
  /// Return a copy of the expression
  virtual SymbolicExpression* copy(void) const = 0;
};

/// Variable in an expression
class VarExpression : public SymbolicExpression {
private:
  CPFloat::CPFloatView v_;
  int index_;
public:
  /// Constructor from a variable
  VarExpression(CPFloatVar v) : v_(CPFloat::CPFloatView(v)) {}
  /// Constructor from a view
  VarExpression(CPFloatView v) : v_(v) {}
  /// Destructor
  virtual ~VarExpression(void) {
    std::cout << "Called destructor of VarExpression" << std::endl;
  }
  /// Output the expression to \a stdout
  virtual void print(void) const {
    std::cout << "var(" << v_ << ")";
  }
  /// Computes the number of variables stored in the expression
  virtual int countVariables(void) const {
    return 1;
  }
  /// Add variables to \a a starting at possition \a i
  virtual void addVariables(Gecode::ViewArray<CPFloatView>& a, int& i) {
    assert(i >= 0 && i <= a.size() && "Invalid size of the array of views");
    for (int k=0;k<i;k++) {
      if (a[k] ==  v_) {
        std::cout << "Debug *** duplicate " << std::endl;
        index_ = k;
        return;
      }
    }
    a[i] = v_;
    index_ = i;
    i++;
  }
  virtual SymbolicExpression* copy(void) const {
    return new VarExpression(v_);
  }
};

/// representation of a symbolic constraint as a relation and two expressions
class SymbolicConstraint {
private:
  /// Left part of the constraint
  SymbolicExpression *left_;
  /// Right part of the constraint
  SymbolicExpression *right_;
  /// Relation stated among the expressions
  RelType rel_;
  // avoid compiler generated constructor
  SymbolicConstraint(void);
public:
  /**
   * \brief Constructor of a constraint (binary) from expressions \a left and
   * \a right.
   *
   * To prevent any dangling pointer cause by the deletion of the expressions in
   * the model, a copy of the expressions is made.
   */
  SymbolicConstraint(SymbolicExpression* left, RelType rel,
                     SymbolicExpression* right)
    : left_(left->copy()), right_(right->copy()), rel_(rel) {}

  SymbolicConstraint(const SymbolicConstraint& c)
    : left_(c.left_->copy()), right_(c.right_->copy()), rel_(c.rel_) {}

  /// Print the constraint to \a std::cout
  void print(void) const {
    std::cout << "SymbCstr(left:";
    left_->print();
    std::cout << " __ right:";
    right_->print();
    std::cout << ")";
  }

  /// Return an array with all the views that appear in the constraint
  Gecode::ViewArray<CPFloatView> views(Gecode::Space& home) const {
    int numViews = left_->countVariables() + right_->countVariables();
    Gecode::ViewArray<CPFloatView> v1(home,numViews);
    int i = 0;
    left_->addViews(v1,i);
    right_->addViews(v1,i);
    Gecode::ViewArray<CPFloatView> v2(home,i);
    for (int k=0;k<i;k++) {
      v2[k] = v1[k];
    }
    return v2;
  }
};
}}

#endif
