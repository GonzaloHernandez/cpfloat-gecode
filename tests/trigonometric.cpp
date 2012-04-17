#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class Trigonometric : public Gecode::Space {
protected:
  CPFloatVar x;
  CPFloatVar y;
  CPFloatVar z;
public:
  Trigonometric(void)  {
    x = CPFloatVar(*this,-10,10);
    y = CPFloatVar(*this,-2.0,2.0);
    z = CPFloatVar(*this,-2.0,2.0);

    sin(*this,x,y);
    cos(*this,x,z);
    equality(*this,y,z);

    branch(*this,x);
    branch(*this,y);
    branch(*this,z);
  }
  void print(std::ostream& os) const {
    os << "\tx:" << x;
    os << " y:" << y;
    os << " z:" << z << std::endl;
  }
  Trigonometric(bool share, Trigonometric& s)
    : Gecode::Space(share,s) {
    x.update(*this, share, s.x);
    y.update(*this,share,s.y);
    z.update(*this,share,s.z);
  }
  virtual Space* copy(bool share) {
    return new Trigonometric(share,*this);
  }
};

int main(int, char**) {
  Trigonometric* g = new Trigonometric();

  Gist::Print<Trigonometric> p("Sin(x) = Cos(x)");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
