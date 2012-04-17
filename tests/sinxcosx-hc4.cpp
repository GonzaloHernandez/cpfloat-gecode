#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/prop-advanced/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class HC4SinxCosx : public Gecode::Space {
protected:
  CPFloatVar x_;
public:
  HC4SinxCosx(void)
    : x_(*this, 0, 3.1416) {
    VarExpression x(x_);

    // sin(x) = cos(x)
    hc4(*this, x.sin() == x.cos() );

    branch(*this,x_);
  }

  void print(std::ostream& os) const {
    os << x_ << std::endl;
  }

  HC4SinxCosx(bool share, HC4SinxCosx& sp)
    : Gecode::Space(share,sp) {
    x_.update(*this, share, sp.x_);
  }

  virtual Space* copy(bool share) {
    return new HC4SinxCosx(share,*this);
  }
};

int main(int, char**) {
  HC4SinxCosx* g = new HC4SinxCosx();

  Gist::Print<HC4SinxCosx> p("Solved for: sin(x)=cos(x)");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
