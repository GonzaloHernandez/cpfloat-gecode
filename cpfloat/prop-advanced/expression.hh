#ifndef __CPFLOAT_EXPRESSION_HH__
#define __CPFLOAT_EXPRESSION_HH__

#include <cpfloat/cpfloat.hh>

#define BOUNDTYPE_MIN -1.7*pow(10.0,308)
#define BOUNDTYPE_MAX 1.7*pow(10.0,308)

using namespace std;
using namespace Gecode;

namespace MPG { namespace CPFloat {

class Expression;
class VarExpression;
class UnaryExpression;
class BinaryExpression;
class Constraint;

//--------------------------------------------------------------------------

enum ClassType {
  EXPR,VAR,CONST,UNARY,BINARY,CONSTRAINT
};

enum ExprType {
  ADD,SUB,MUL,DIV,POW,  // assert of modify (-5) UnaryExpression::print()
  SIN,COS,TAN,
  SQRT
};

enum RelType {
  EQUAL
};

//--------------------------------------------------------------------------

class Expression {
protected:
  INTERVAL interval_;
  Expression();
public:
  virtual ~Expression();
  virtual ClassType getClass() const;
  virtual Expression& clone();
  virtual Expression& cloneToUpdate();
  virtual Expression& cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target);
  virtual void  print() const;
  virtual void  subscribe(Space& home, Propagator& p, PropCond pc, bool schedule=true);
  virtual void  cancel(Space& home, Propagator& p, PropCond pc);
  virtual void  update(Space& home, bool share, Expression& expr);
  virtual bool  assigned();
  virtual int   countViews();
  virtual void  collectViews(ViewArray<CPFloatView>& views,int& i);
  virtual INTERVAL evaluate();
  virtual Gecode::ExecStatus propagate(Gecode::Space& home,INTERVAL interval);
  INTERVAL getInteval();
  Expression& sin();
  Expression& cos();
  Expression& sqrt();
  Expression& operator+ (Expression& expr);
  Expression& operator+ (BoundType expr);
  Expression& operator- (Expression& expr);
  Expression& operator- (BoundType expr);
  Expression& operator* (Expression& expr);
  Expression& operator* (BoundType expr);
  Expression& operator/ (Expression& expr);
  Expression& operator/ (BoundType expr);
  Expression& operator^ (unsigned int exponent);
  Constraint& operator==(Expression& expr);
  Constraint& operator==(BoundType expr);
};

//--------------------------------------------------------------------------

class VarExpression : public Expression {
private:
  CPFloat::CPFloatView v_;
public:
  VarExpression(CPFloatVar v);
  VarExpression(CPFloatView v);
  ~VarExpression();
  ClassType getClass() const;
  Expression& clone();
  Expression& cloneToUpdate();
  Expression& cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target);
  void  print() const;
  void  subscribe(Space& home, Propagator& p, PropCond pc, bool schedule=true);
  void  cancel(Space& home, Propagator& p, PropCond pc);
  void  update(Space& home, bool share, Expression& expr);
  bool  assigned();
  int   countViews();
  void  collectViews(ViewArray<CPFloatView>& views,int& i);
  INTERVAL evaluate();
  Gecode::ExecStatus propagate(Gecode::Space& home,INTERVAL interval);
};

//--------------------------------------------------------------------------

class ConstExpression : public Expression {
private:
  BoundType v_;
public:
  ConstExpression(BoundType v);
  ~ConstExpression();
  ClassType getClass() const;
  Expression& clone();
  Expression& cloneToUpdate();
  Expression& cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target);
  void  print() const;
  void  subscribe(Space& home, Propagator& p, PropCond pc, bool schedule=true);
  void  cancel(Space& home, Propagator& p, PropCond pc);
  void  update(Space& home, bool share, Expression& expr);
  bool  assigned();
  int   countViews();
  void  collectViews(ViewArray<CPFloatView>& views,int& i);
  INTERVAL evaluate();
  Gecode::ExecStatus propagate(Gecode::Space& home,INTERVAL interval);
};

//--------------------------------------------------------------------------

class UnaryExpression : public Expression {
private:
  Expression& expr_;
  ExprType    type_;
  UnaryExpression(Expression& expr, ExprType type);
public:
  ~UnaryExpression();
  ClassType getClass() const;
  Expression& clone();
  Expression& cloneToUpdate();
  Expression& cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target);
  void  print() const;
  void  subscribe(Space& home, Propagator& p, PropCond pc, bool schedule=true);
  void  cancel(Space& home, Propagator& p, PropCond pc);
  void  update(Space& home, bool share, Expression& expr);
  bool  assigned();
  int   countViews();
  void  collectViews(ViewArray<CPFloatView>& views,int& i);
  INTERVAL evaluate();
  Gecode::ExecStatus propagate(Gecode::Space& home,INTERVAL interval);
friend class Expression;
};

//--------------------------------------------------------------------------

class BinaryExpression : public Expression {
private:
  Expression& left_;
  Expression& right_;
  ExprType    type_;
  BinaryExpression(Expression& left, Expression& right, ExprType type);
public:
  ~BinaryExpression();
  ClassType getClass() const;
  Expression& clone();
  Expression& cloneToUpdate();
  Expression& cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target);
  void  print() const;
  void  subscribe(Space& home, Propagator& p, PropCond pc, bool schedule=true);
  void  cancel(Space& home, Propagator& p, PropCond pc);
  void  update(Space& home, bool share, Expression& expr);
  bool  assigned();
  int   countViews();
  void  collectViews(ViewArray<CPFloatView>& views,int& i);
  INTERVAL evaluate();
  Gecode::ExecStatus propagate(Gecode::Space& home,INTERVAL interval);
friend class Expression;
friend Expression& operator+(BoundType c,Expression& expr);
friend Expression& operator-(BoundType c,Expression& expr);
friend Expression& operator*(BoundType c,Expression& expr);
friend Expression& operator/(BoundType c,Expression& expr);
};

//--------------------------------------------------------------------------

class Constraint {
private:
  Expression& left_;
  Expression& right_;
  RelType     type_;
  Constraint(Expression& left, Expression& right, RelType type);
public:
  ~Constraint();
  ClassType getClass() const;
  Constraint& clone();
  Constraint& cloneToUpdate();
  Constraint& cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target);
  void  print() const;
  void  subscribe(Space& home, Propagator& p, PropCond pc, bool schedule=true);
  void  cancel(Space& home, Propagator& p, PropCond pc);
  void  update(Space& home, bool share, Constraint& cnst);
  bool  assigned();
  int   countViews();
  void  collectViews(ViewArray<CPFloatView>& views);
  void  evaluate();
  Gecode::ExecStatus propagate(Gecode::Space& home);
friend class Expression;
friend Constraint& operator==(BoundType c,Expression& expr);
};

}}

//--------------------------------------------------------------------------

namespace MPG { namespace CPFloat {

forceinline
Expression::Expression() {
}

forceinline
Expression::~Expression() {
}

forceinline
ClassType Expression::getClass() const {
  return EXPR;
}

forceinline
Expression& Expression::clone() {
  cout << "*** Warning *** Expression Method used (clone())" << endl;
  return *(new Expression());
}

forceinline
Expression& Expression::cloneToUpdate() {
  cout << "*** Warning *** Expression Method used (cloneToUpdate())" << endl;
  return *(new Expression());
}

forceinline
Expression& Expression::cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target) {
  cout << "*** Warning *** Expression Method used (cloneWithReplace(...))" << endl;
  return *(new Expression());
}

forceinline
void Expression::print() const {
  cout << " *** Warning *** Expression Method used (print())" << endl;
}

forceinline
void Expression::subscribe(Space& , Propagator& , PropCond , bool ) {
  cout << " *** Warning *** Expression Method used (subscribe(...))" << endl;
}

forceinline
void Expression::cancel(Space& , Propagator& , PropCond ) {
  cout << " *** Warning *** Expression Method used (cancel(...))" << endl;
}

forceinline
void Expression::update(Space& , bool , Expression& ) {
  cout << " *** Warning *** Expression Method used (update(...))" << endl;
}

forceinline
INTERVAL Expression::getInteval() {
  return interval_;
}

forceinline
Expression& Expression::sin() {
  Expression& expr = getClass()==VAR ? this->clone() : *this;
  return *(new UnaryExpression(expr,SIN));
}

forceinline
Expression& Expression::cos() {
  Expression& expr = getClass()==VAR ? this->clone() : *this;
  return *(new UnaryExpression(expr,COS));
}

forceinline
Expression& Expression::sqrt() {
  Expression& expr = getClass()==VAR ? this->clone() : *this;
  return *(new UnaryExpression(expr,SQRT));
}

forceinline
Expression& Expression::operator+(Expression& expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,ADD));
}

forceinline
Expression& Expression::operator+(BoundType expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = *(new ConstExpression(expr));
  return *(new BinaryExpression(left,right,ADD));
}

forceinline
Expression& operator+(BoundType c,Expression& expr) {
  Expression& left = *(new ConstExpression(c));
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,ADD));
}

forceinline
Expression& Expression::operator-(Expression& expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,SUB));
}

forceinline
Expression& Expression::operator-(BoundType expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = *(new ConstExpression(expr));
  return *(new BinaryExpression(left,right,SUB));
}

forceinline
Expression& operator-(BoundType c,Expression& expr) {
  Expression& left = *(new ConstExpression(c));
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,SUB));
}

forceinline
Expression& Expression::operator*(Expression& expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,MUL));
}

forceinline
Expression& Expression::operator*(BoundType expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = *(new ConstExpression(expr));
  return *(new BinaryExpression(left,right,MUL));
}

forceinline
Expression& operator*(BoundType c,Expression& expr) {
  Expression& left = *(new ConstExpression(c));
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,MUL));
}

forceinline
Expression& Expression::operator/(Expression& expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,DIV));
}

forceinline
Expression& Expression::operator/(BoundType expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = *(new ConstExpression(expr));
  return *(new BinaryExpression(left,right,DIV));
}

forceinline
Expression& operator/(BoundType c,Expression& expr) {
  Expression& left = *(new ConstExpression(c));
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new BinaryExpression(left,right,DIV));
}

forceinline
Expression& Expression::operator^(unsigned int exponent) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = *(new ConstExpression(exponent));
  return *(new BinaryExpression(left,right,POW));
}

forceinline
Constraint& Expression::operator==(Expression& expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new Constraint(left,right,EQUAL));
}

forceinline
Constraint& Expression::operator==(BoundType expr) {
  Expression& left = getClass()==VAR ? this->clone() : *this;
  Expression& right = *(new ConstExpression(expr));
  return *(new Constraint(left,right,EQUAL));
}

forceinline
Constraint& operator==(BoundType c,Expression& expr) {
  Expression& left = *(new ConstExpression(c));
  Expression& right = expr.getClass()==VAR ? expr.clone() : expr;
  return *(new Constraint(left,right,EQUAL));
}

forceinline
bool Expression::assigned() {
  cout << " *** Warning *** Expression Method used (assigned(...))" << endl;
  return false;
}

forceinline
int Expression::countViews() {
  cout << " *** Warning *** Expression Method used (countViews(...))" << endl;
  return -1;
}

forceinline
void Expression::collectViews(ViewArray<CPFloatView>&,int&) {
  cout << " *** Warning *** Expression Method used (collectViews(...))" << endl;
}

forceinline
INTERVAL Expression::evaluate() {
  cout << " *** Warning *** Expression Method used (evaluate(...))" << endl;
  return interval_;
}

forceinline
Gecode::ExecStatus Expression::propagate(Gecode::Space& ,INTERVAL ) {
  cout << " *** Warning *** Expression Method used (propagate(...))" << endl;
  return Gecode::ES_NOFIX;
}

//--------------------------------------------------------------------------

forceinline
VarExpression::VarExpression(CPFloatVar v)
  : Expression(),v_(CPFloat::CPFloatView(v)) {
}

forceinline
VarExpression::VarExpression(CPFloatView v)
  : Expression(),v_(v) {
}

forceinline
VarExpression::~VarExpression() {
}

forceinline
ClassType VarExpression::getClass() const {
  return VAR;
}

forceinline
Expression& VarExpression::clone() {
  VarExpression* e = new VarExpression(v_);
  return *(e);
}

forceinline
Expression& VarExpression::cloneToUpdate() {
  VarExpression* e = new VarExpression(CPFloat::CPFloatView());
  return *(e);
}

forceinline
Expression& VarExpression::cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target) {
  VarExpression* e;
  if (v_ == source) {
    e = new VarExpression(target);
  }
  else {
    e = new VarExpression(CPFloat::CPFloatView(CPFloatVar(home,v_.glb(),v_.lub())));
  }
  return *(e);
}

forceinline
void VarExpression::print() const {
  std::cout << v_ ;
}

forceinline
void VarExpression::subscribe(Space& home, Propagator& p, PropCond pc, bool schedule) {
  v_.subscribe(home,p,pc,schedule);
}


forceinline
void VarExpression::cancel(Space& home, Propagator& p, PropCond pc) {
  v_.cancel(home,p,pc);
}

forceinline
void VarExpression::update(Space& home, bool share, Expression& expr) {
  VarExpression& v = (VarExpression&)expr;
  v_.update(home,share,v.v_);
}

forceinline
bool VarExpression::assigned() {
  return v_.assigned();
}

forceinline
int VarExpression::countViews() {
  return 1;
}

forceinline
void VarExpression::collectViews(ViewArray<CPFloatView>& views,int& i) {
  views[i++] = v_;
}

forceinline
INTERVAL VarExpression::evaluate() {
  interval_.lo = v_.glb();
  interval_.hi = v_.lub();
  return interval_;
}

forceinline
Gecode::ExecStatus VarExpression::propagate(Gecode::Space& home,INTERVAL interval) {
  interval_ = interval;
  GECODE_ME_CHECK(v_.leq(home,interval_.hi));
  GECODE_ME_CHECK(v_.geq(home,interval_.lo));
  return Gecode::ES_NOFIX;
}

//--------------------------------------------------------------------------

forceinline
ConstExpression::ConstExpression(BoundType v)
  : Expression(),v_(v) {
}

forceinline
ConstExpression::~ConstExpression() {
}

forceinline
ClassType ConstExpression::getClass() const {
  return CONST;
}

forceinline
Expression& ConstExpression::clone() {
  return *(new ConstExpression(v_));
}

forceinline
Expression& ConstExpression::cloneToUpdate() {
  return *(new ConstExpression(v_));
}

forceinline
Expression& ConstExpression::cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target) {
  return *(new ConstExpression(v_));
}

forceinline
void ConstExpression::print() const {
  std::cout << v_ ;
}

forceinline
void ConstExpression::subscribe(Space& , Propagator& , PropCond , bool ) {
}

forceinline
void ConstExpression::cancel(Space& , Propagator& , PropCond ) {
}

forceinline
void ConstExpression::update(Space& , bool , Expression& ) {
}

forceinline
bool ConstExpression::assigned() {
  return true;
}

forceinline
int ConstExpression::countViews() {
  return 0;
}

forceinline
void ConstExpression::collectViews(ViewArray<CPFloatView>& ,int&) {
}

forceinline
INTERVAL ConstExpression::evaluate() {
  interval_.lo = v_;
  interval_.hi = v_;
  return interval_;
}

forceinline
Gecode::ExecStatus ConstExpression::propagate(Gecode::Space& ,INTERVAL ) {
  return Gecode::ES_NOFIX;
}

//--------------------------------------------------------------------------

forceinline
UnaryExpression::UnaryExpression(Expression &expr, ExprType type)
  : Expression(), expr_(expr), type_(type) {
}

forceinline
UnaryExpression::~UnaryExpression() {
  delete &expr_;
}

forceinline
ClassType UnaryExpression::getClass() const {
  return UNARY;
}

forceinline
Expression& UnaryExpression::clone() {
  Expression& expr = expr_.clone();
  ExprType    type = type_;
  return *(new UnaryExpression(expr,type));
}

forceinline
Expression& UnaryExpression::cloneToUpdate() {
  Expression& expr = expr_.cloneToUpdate();
  ExprType    type = type_;
  return *(new UnaryExpression(expr,type));
}

forceinline
Expression& UnaryExpression::cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target) {
  Expression& expr = expr_.cloneWithReplace(home,source,target);
  ExprType    type = type_;
  return *(new UnaryExpression(expr,type));
}

forceinline
void UnaryExpression::print() const {
  string type[]= {"sin","cos","tan",
                  "sqrt"};
  cout << type[type_-5] << "(";
  expr_.print();
  cout << ")";
}

forceinline
void UnaryExpression::subscribe(Space& home, Propagator& p, PropCond pc, bool schedule) {
  expr_.subscribe(home,p,pc,schedule);
}

forceinline
void UnaryExpression::cancel(Space& home, Propagator& p, PropCond pc) {
  expr_.cancel(home,p,pc);
}

forceinline
void UnaryExpression::update(Space& home, bool share, Expression& expr) {
  UnaryExpression& u = (UnaryExpression&)expr;
  expr_.update(home,share,u.expr_);
}

forceinline
bool UnaryExpression::assigned() {
  return expr_.assigned();
}

forceinline
int UnaryExpression::countViews() {
  return expr_.countViews();
}

forceinline
void UnaryExpression::collectViews(ViewArray<CPFloatView>& views,int& i) {
  expr_.collectViews(views,i);
}

forceinline
INTERVAL UnaryExpression::evaluate() {
  INTERVAL x = expr_.evaluate();
  INTERVAL z;
  z.lo = -1;
  z.hi = 1;

  switch(type_) {
  case SIN:
    narrow_sin(&x,&z);
    break;
  case COS:
    narrow_cos(&x,&z);
    break;
  case SQRT:
    z = sqrtII(x);
    break;
  default:
    break;
  }
  interval_ = z;
  return z;
}

forceinline
Gecode::ExecStatus UnaryExpression::propagate(Gecode::Space& home,INTERVAL interval) {
  INTERVAL z = interval;
  INTERVAL x = expr_.getInteval();

  switch(type_) {
  case SIN:
    intersect_inv_sinII(z,&x);  //sin(x)=z
    break;
  case COS:
    intersect_inv_cosII(z,&x);  //cos(x)=z
    break;
  case SQRT:
    x = intersectIII(x,squareII(z)); //sqrt(x)=z
    break;
  default:
    break;
  }

  GECODE_ME_CHECK(expr_.propagate(home,x));

  return Gecode::ES_NOFIX;
}

//--------------------------------------------------------------------------

forceinline
BinaryExpression::BinaryExpression(Expression &left, Expression &right, ExprType type)
  : Expression(), left_(left), right_(right), type_(type) {
}

forceinline
BinaryExpression::~BinaryExpression() {
  delete &left_;
  delete &right_;
}

forceinline
ClassType BinaryExpression::getClass() const {
  return BINARY;
}

forceinline
Expression& BinaryExpression::clone() {
  Expression& left = left_.clone();
  Expression& right = right_.clone();
  ExprType    type = type_;
  return *(new BinaryExpression(left,right,type));
}

forceinline
Expression& BinaryExpression::cloneToUpdate() {
  Expression& left = left_.cloneToUpdate();
  Expression& right = right_.cloneToUpdate();
  ExprType    type = type_;
  return *(new BinaryExpression(left,right,type));
}

forceinline
Expression& BinaryExpression::cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target) {
  Expression& left = left_.cloneWithReplace(home,source,target);
  Expression& right = right_.cloneWithReplace(home,source,target);
  ExprType    type = type_;
  return *(new BinaryExpression(left,right,type));
}

forceinline
void BinaryExpression::print() const {
  char type[] = "+-*/^";
  cout << "(";
  left_.print();
  cout << type[type_];
  right_.print();
  cout << ")";
}

forceinline
void BinaryExpression::subscribe(Space& home, Propagator& p, PropCond pc, bool schedule) {
  left_.subscribe(home,p,pc,schedule);
  right_.subscribe(home,p,pc,schedule);
}

forceinline
void BinaryExpression::cancel(Space& home, Propagator& p, PropCond pc) {
  left_.cancel(home,p,pc);
  right_.cancel(home,p,pc);
}

forceinline
void BinaryExpression::update(Space& home, bool share, Expression& expr) {
  BinaryExpression& b = (BinaryExpression&)expr;
  left_.update(home,share,b.left_);
  right_.update(home,share,b.right_);
}

forceinline
bool BinaryExpression::assigned() {
  return left_.assigned() && right_.assigned();
}

forceinline
int BinaryExpression::countViews() {
  return left_.countViews() + right_.countViews();
}

forceinline
void BinaryExpression::collectViews(ViewArray<CPFloatView>& views,int& i) {
  left_.collectViews(views,i);
  right_.collectViews(views,i);
}

forceinline
INTERVAL BinaryExpression::evaluate() {
  INTERVAL left  = left_.evaluate();
  INTERVAL right = right_.evaluate();

  switch(type_) {
  case ADD:
    interval_.lo = left.lo + right.lo;
    interval_.hi = left.hi + right.hi;
    break;

  case SUB:
    interval_.lo = left.lo - right.hi;
    interval_.hi = left.hi - right.lo;
    break;

  case MUL:
    interval_.lo = BOUNDTYPE_MIN;
    interval_.hi = BOUNDTYPE_MAX;
    intersect_mulIII(left,right,&interval_);
    break;

  case DIV:
    interval_.lo = BOUNDTYPE_MIN;
    interval_.hi = BOUNDTYPE_MAX;
    intersect_divIII(left,right,&interval_);
    break;

  case POW:
    interval_.lo = BOUNDTYPE_MIN;
    interval_.hi = BOUNDTYPE_MAX;
    if ((int)right.lo%2 == 0) {
      if (right.lo == 0.0) {
        interval_.lo = 1.0;
        interval_.hi = 1.0;
      }
      else if (right.lo == 2.0) {
        interval_ = squareII(left);
        if (interval_.lo<0.0) interval_.lo = 0.0;
      }
      else {
        narrow_pow_even(&left,&right,&interval_);
      }
    }
    else {
      if (right.lo == 1.0) {
        interval_ = right;
      }
      else {
        narrow_pow_odd(&left,&right,&interval_);
      }
    }
    break;

  default:
    break;
  }
  return interval_;
}

forceinline
Gecode::ExecStatus BinaryExpression::propagate(Gecode::Space& home,INTERVAL interval) {
  INTERVAL z = interval;
  INTERVAL x = left_.getInteval();
  INTERVAL y = right_.getInteval();

  switch(type_) {
  case ADD:
    x = intersectIII(x,subIII(z,y));  //x=z-y
    y = intersectIII(y,subIII(z,x));  //y=z-x
    break;

  case SUB:
    x = intersectIII(x,addIII(z,y));  //x=z+y
    y = intersectIII(y,subIII(x,z));  //y=x-z
    break;

  case MUL:
    intersect_divIII(z,y,&x); //x=z/y
    intersect_divIII(z,x,&y); //y=z/x
    break;

  case DIV:
    intersect_mulIII(z,y,&x); //x=z*y
    intersect_mulIII(x,z,&y); //y=x/z
    break;

  case POW:
    if ((int)y.lo%2==0) {
      if (y.lo == 2.0) {
        narrow_square(&x,&z); //x^2=z
      }
      else {
        narrow_pow_even(&x,&y,&z);  //x^y=z | y is even
      }
    }
    else {
      narrow_pow_odd(&x,&y,&z); //x^y=z | y is odd
    }
    break;

  default:
    break;
  }

  GECODE_ME_CHECK(left_.propagate(home,x));
  GECODE_ME_CHECK(right_.propagate(home,y));

  return Gecode::ES_NOFIX;
}

//--------------------------------------------------------------------------

forceinline
Constraint::Constraint(Expression& left, Expression& right, RelType type)
  : left_(left), right_(right), type_(type) {
}

forceinline
Constraint::~Constraint() {
  delete &left_;
  delete &right_;
}

forceinline
ClassType Constraint::getClass() const {
  return CONSTRAINT;
}

forceinline
Constraint& Constraint::clone() {
  Expression& left = left_.clone();
  Expression& right = right_.clone();
  RelType     type = type_;
  return *(new Constraint(left,right,type));
}

forceinline
Constraint& Constraint::cloneToUpdate() {
  Expression& left = left_.cloneToUpdate();
  Expression& right = right_.cloneToUpdate();
  RelType     type = type_;
  return *(new Constraint(left,right,type));
}

forceinline
Constraint& Constraint::cloneWithReplace(Space& home, CPFloatView& source,CPFloatView& target) {
  Expression& left = left_.cloneWithReplace(home,source,target);
  Expression& right = right_.cloneWithReplace(home,source,target);
  RelType     type = type_;
  return *(new Constraint(left,right,type));
}


forceinline
void Constraint::print() const {
  char type[] = "=";
  left_.print();
  cout << type[type_];
  right_.print();
}

forceinline
void Constraint::subscribe(Space& home, Propagator& p, PropCond pc, bool schedule) {
  left_.subscribe(home,p,pc,schedule);
  right_.subscribe(home,p,pc,schedule);
}

forceinline
void Constraint::cancel(Space& home, Propagator& p, PropCond pc) {
  left_.cancel(home,p,pc);
  right_.cancel(home,p,pc);
}

forceinline
void Constraint::update(Space& home, bool share, Constraint& cnst) {
  left_.update(home,share,cnst.left_);
  right_.update(home,share,cnst.right_);
}

forceinline
bool Constraint::assigned() {
  return left_.assigned() && right_.assigned();
}

forceinline
int Constraint::countViews() {
  return left_.countViews() + right_.countViews();
}

forceinline
void Constraint::collectViews(ViewArray<CPFloatView>& views) {
  int i = 0;
  left_.collectViews(views,i);
  right_.collectViews(views,i);
}

forceinline
void Constraint::evaluate() {
  left_.evaluate();
  right_.evaluate();
}

forceinline
Gecode::ExecStatus Constraint::propagate(Gecode::Space& home) {
  INTERVAL left = left_.getInteval();
  INTERVAL right = right_.getInteval();

  switch(type_) {
  case EQUAL:
    narrow_eq(&left,&right);
    GECODE_ME_CHECK(left_.propagate(home,left));
    GECODE_ME_CHECK(right_.propagate(home,right));
    break;
  default:
    break;
  }
  return Gecode::ES_NOFIX;
}

//--------------------------------------------------------------------------

}}

#endif // HC4_H
