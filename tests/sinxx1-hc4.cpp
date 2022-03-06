#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class HC4Sinxx : public Gecode::Space {
protected:
  CPFloatVar x_;
public:
  HC4Sinxx(void)
    : x_(*this, -10, 10) {
    VarExpression x(x_);

    hc4(*this, (((3.0*(x^2)*x - 5.0*x)+2.0)*(x.sin()^2)) + (x*(x^2)+5.0*x)*x.sin() == 2.0*(x^2)+x+2.0 );

    branch(*this,x_);
  }

  void print(std::ostream& os) const {
    os << x_ << std::endl;
  }

  HC4Sinxx(bool share, HC4Sinxx& sp)
    : Gecode::Space(share,sp) {
    x_.update(*this, share, sp.x_);
  }

  virtual Space* copy(bool share) {
    return new HC4Sinxx(share,*this);
  }
};

int main(int, char**) {
  HC4Sinxx* g = new HC4Sinxx();

  Gist::Print<HC4Sinxx> p("Solved for: Sin long expression");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
