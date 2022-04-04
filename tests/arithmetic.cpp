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

#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>

using namespace Gecode;

using namespace MPG;
using namespace MPG::CPFloat;

class Arithmetic : public Gecode::Space {
protected:
  CPFloatVar x;
  CPFloatVar y;
  CPFloatVar z;
public:
  Arithmetic(void)  {
    x = CPFloatVar(*this, 0.9, 1.1);
    y = CPFloatVar(*this, 0.9, 1.1);
    z = CPFloatVar(*this, 1.9, 2.1);

    addition(*this,x,y,z);
    branch(*this,x);
    branch(*this,y);
    branch(*this,z);
  }

  void print(std::ostream& os) const {
    os << "xyz" << x << y <<  z << std::endl;
  }

  Arithmetic(bool share, Arithmetic& s)
    : Gecode::Space(share,s) {
    x.update(*this,share,s.x);
    y.update(*this,share,s.y);
    z.update(*this,share,s.z);
  }

  virtual Space* copy(bool share) {
    return new Arithmetic(share,*this);
  }
};

int main(int, char**) {
  Arithmetic* g = new Arithmetic();

  Gecode::DFS<Arithmetic> e(g);
  if (Arithmetic* s = e.next()) {
      s->print(std::cout);
      delete s;
  }
  delete g;

//  Gist::Print<Arithmetic> p("x * y = z | y = x");
//  Gist::Options o;
//  o.inspect.click(&p);
//  Gist::dfs(g,o);
//  delete g;

  return 0;
}
