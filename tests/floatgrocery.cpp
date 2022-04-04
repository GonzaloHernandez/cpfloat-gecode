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
#include <vector>

using namespace Gecode;

using namespace MPG;
using namespace MPG::CPFloat;

class FloatGrocery : public Gecode::Space {
protected:
  CPFloatVar w;
  CPFloatVar x;
  CPFloatVar y;
  CPFloatVar z;
public:
  FloatGrocery(void)  {
    w = CPFloatVar(*this, 0.1, 7.11);
    x = CPFloatVar(*this, 0.1, 7.11);
    y = CPFloatVar(*this, 0.1, 7.11);
    z = CPFloatVar(*this, 0.1, 7.11);

    CPFloatVar a1 = CPFloatVar(*this, 0.1, 7.11);
    CPFloatVar a2 = CPFloatVar(*this, 0.1, 7.11);
    CPFloatVar a3 = CPFloatVar(*this, 0.1, 7.11);
    CPFloatVar t1 = CPFloatVar(*this, 0.1, 7.11);
    CPFloatVar t2 = CPFloatVar(*this, 0.1, 7.11);
    CPFloatVar t3 = CPFloatVar(*this, 0.1, 7.11);
    CPFloatVar r  = CPFloatVar(*this, 7.11, 7.11);
    
    addition(*this,w,x,a1);
    addition(*this,a1,y,a2);
    addition(*this,a2,z,a3);

    times(*this,w,x,t1);
    times(*this,t1,y,t2);
    times(*this,t2,z,t3);

    equality(*this,a3,t3);
    equality(*this,t3,r);
    
    branch(*this,w);
    branch(*this,x);
    branch(*this,y);
    branch(*this,z);
  }
  void print(std::ostream& os) const {
    os << "\tSol w:" << w << " x:" << x <<  " y:" << y <<  " z:" << z << std::endl;
  }
  FloatGrocery(bool share, FloatGrocery& s)
    : Gecode::Space(share,s) {
    w.update(*this,share,s.w);
    x.update(*this,share,s.x);
    y.update(*this,share,s.y);
    z.update(*this,share,s.z);
  }
  virtual Space* copy(bool share) {
    return new FloatGrocery(share,*this);
  }
};

int main(int, char**) {
  FloatGrocery* g = new FloatGrocery();

  Gist::Print<FloatGrocery> p("Grocery - First Try");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
