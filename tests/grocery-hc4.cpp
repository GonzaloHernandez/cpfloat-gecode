#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/prop-advanced/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class HC4Grocery : public Gecode::Space {
protected:
  CPFloatVarArray a_;
public:
  HC4Grocery(void)
    : a_(*this, 4, 0.0, 7.11) {
    VarExpression w(a_[0]), x(a_[1]), y(a_[2]), z(a_[3]);

    // Grocery
    hc4(*this, w + x + y + z == 7.11 );
    hc4(*this, w * x * y * z == 7.11 );

    randselection(*this,a_);
  }

  void print(std::ostream& os) const {
    os << a_ << std::endl;
  }

  HC4Grocery(bool share, HC4Grocery& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new HC4Grocery(share,*this);
  }
};

int main(int, char**) {
  HC4Grocery* g = new HC4Grocery();

  Gist::Print<HC4Grocery> p("Solved for: Grocery problem");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
