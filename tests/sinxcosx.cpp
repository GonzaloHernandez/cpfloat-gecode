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

class SinxCosx : public Gecode::Space {
protected:
  CPFloatVar x;
  CPFloatVar y;
  CPFloatVar z;
public:
  SinxCosx(void)  {
    x = CPFloatVar(*this,0.0,3.1415);
    y = CPFloatVar(*this,-1.0,1.0);
    z = CPFloatVar(*this,-1.0,1.0);

    sin(*this,x,y);
    cos(*this,x,z);
    equality(*this,y,z);
    branch(*this,x);
  }

  void print(std::ostream& os) const {
    os << "\tsol x: " << x << std::endl;
    os << "\t y: " << y << std::endl;
    os << "\t z: " << z << std::endl;
  }

  SinxCosx(bool share, SinxCosx& s)
    : Gecode::Space(share,s) {
    x.update(*this, share, s.x);
    y.update(*this,share,s.y);
    z.update(*this,share,s.z);
  }

  virtual Space* copy(bool share) {
    return new SinxCosx(share,*this);
  }
};

int main(int, char**) {
  SinxCosx* g = new SinxCosx();

  /*
  Gecode::DFS<SinxCosx> e(g);
  delete g;
  std::cout << "Search will start" << std::endl;
  while (Gecode::Space* s = e.next()) {
    static_cast<SinxCosx*>(s)->print(std::cout);
    delete s;
  }
*/

  Gist::Print<SinxCosx> p("Sin(x) = Cos(x)");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
