/*
 *  Authors:
 *     Gonzalo Hernández <gonzalohernandez@udenar.edu.co>
 *     Gustavo Gutierrez
 *
 *  Year of last major update
 *     2011
 * 
 *  This file is a complement of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

namespace MPG { namespace CPFloat {

Expression::Expression() {
}

Expression::~Expression() {
}

ClassType Expression::getClass() const {
  return EXPR;
}

void Expression::print() const {
  cout << "*** Warning *** Expression Method used (print())" << endl;
}

Expression& Expression::sin(){
  Expression& expr  = *this;
  return *(new UnaryExpression(expr,SIN));
}

Expression& Expression::operator+(Expression& expr){
  Expression& left  = *this;
  Expression& right = expr;
  return *(new BinaryExpression(left,right,ADD));
}

Constraint& Expression::operator==(Expression& expr) {
  Expression& left  = *this;
  Expression& right = expr;
  return *(new Constraint(left,right,EQUAL));
}

//--------------------------------------------------------------------------

VarExpression::VarExpression(CPFloatVar v)
  : Expression(),v_(CPFloat::CPFloatView(v)) {
}

VarExpression::~VarExpression() {
  cout << "*** Destroying VarExpression *** ";
  print();
  cout  << endl;
}

ClassType VarExpression::getClass() const {
  return VAR;
}

void VarExpression::print() const {
  //cout << "[" << _lb << "," << _ub << "]";
  std::cout << "var(" << v_ << ")";
}

//--------------------------------------------------------------------------

UnaryExpression::UnaryExpression(Expression &expr, ExprType type)
  : Expression(), expr_(expr), type_(type) {
}

UnaryExpression::~UnaryExpression() {
  cout << "*** Destroying UnaryExpression *** ";
  print();
  cout  << endl;
  if (expr_.getClass()!=VAR) delete &expr_;
}

ClassType UnaryExpression::getClass() const {
  return UNARY;
}

void UnaryExpression::print() const {
  string type[]= {"sin","cos","tan"};
  cout << type[type_-5] << "(";
  expr_.print();
  cout << ")";
}

//--------------------------------------------------------------------------

BinaryExpression::BinaryExpression(Expression &left, Expression &right, ExprType type)
  : Expression(), left_(left), right_(right), type_(type) {
}

BinaryExpression::~BinaryExpression() {
  cout << "*** Destroying BinaryExpression *** ";
  print();
  cout  << endl;
  if (left_.getClass()!=VAR) delete &left_;
  if (right_.getClass()!=VAR) delete &right_;
}

ClassType BinaryExpression::getClass() const {
  return BINARY;
}

void BinaryExpression::print() const {
  char type[] = "+-*/";
  cout << "(";
  left_.print();
  cout << type[type_];
  right_.print();
  cout << ")";
}

//--------------------------------------------------------------------------

Constraint::Constraint(Expression& left, Expression& right, RelType type)
  : left_(left), right_(right), type_(type) {
}

Constraint::~Constraint() {
  cout << "*** Destroying Constraint *** ";
  print();
  cout  << endl;
  if (left_.getClass()!=VAR) delete &left_;
  if (right_.getClass()!=VAR) delete &right_;
}

ClassType Constraint::getClass() const {
  return CONSTRAINT;
}

void Constraint::print() const {
char type[] = "=";
  left_.print();
  cout << type[type_];
  right_.print();
}

//--------------------------------------------------------------------------

void hc4(Constraint& cnst) {
  cnst.print(); cout << endl;
  delete &cnst;
}

}}

