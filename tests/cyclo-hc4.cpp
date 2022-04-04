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
#include <cpfloat/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class HC4Cyclo : public Gecode::Space {
protected:
  CPFloatVarArray a_;
public:
  HC4Cyclo(void)
    : a_(*this, 3, 0.0, 10.0) {
    VarExpression x(a_[0]), y(a_[1]), z(a_[2]);

//    // Cyclo Version XRI
//    hc4(*this, y*z*24.0 - (y^2)*(z^2) - (y^2) - (z^2) - 13.0 == 0.0 );
//    hc4(*this, x*z*24.0 - (z^2)*(z^2) - (x^2) - (z^2) - 13.0 == 0.0 );
//    hc4(*this, x*y*24.0 - (x^2)*(y^2) - (x^2) - (y^2) - 13.0 == 0.0 );

    // Cyclo Version COCONUT
    hc4(*this, 959*(y^2)+774*(z^2)+1389*y*z+1313*(y^2)*(z^2) == 310);
    hc4(*this, 755*(z^2)+917*(x^2)+1451*z*x+269*(z^2)*(x^2)  == 365);
    hc4(*this, 837*(x^2)+838*(y^2)+1655*x*y+1352*(x^2)*(y^2) == 413);

    randselection(*this,a_);
  }

  void print(std::ostream& os) const {
    os << "x: " << a_[0] << std::endl <<
          "y: " << a_[1] << std::endl <<
          "z: " << a_[2] << std::endl;
  }

  HC4Cyclo(bool share, HC4Cyclo& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new HC4Cyclo(share,*this);
  }
};

int main(int, char**) {
  HC4Cyclo* g = new HC4Cyclo();

  Gist::Print<HC4Cyclo> p("Solved for: Cyclo");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
