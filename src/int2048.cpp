#include "int2048.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <cinttypes>
#include <cstdio>

namespace sjtu {
  void int2048::fft(std::vector <std::complex<double>> &A,int type) {
    static std::vector <int> pr;
    static std::vector <std::vector<std::complex<double>>> roots[2];
    int up=A.size();
    if((int)pr.size()!=up) {
    pr.assign(up,0);
    for(int i=0;i<up;i++) {
		  pr[i]=pr[i>>1]>>1;
		  if(i&1) pr[i]|=(up>>1);
	  } 
    }
    const double PI=3.1415926535897932384626433832795028841971;
	  for(int i=0;i<up;i++) if(i<pr[i]) swap(A[i],A[pr[i]]);
	  for(int mid=1;mid<up;mid<<=1) {
		  std::complex <double> wn=std::complex <double>(cos(PI/mid),type*sin(PI/mid));
      int level=0;
      for(int k=mid;k>1;k>>=1) level++;
      auto &cache=roots[type==1?0:1];
      if((int)cache.size()<=level) cache.resize(level+1);
      auto &rts=cache[level];
      if(rts.empty()) {
        rts.resize(mid);
        std::complex <double> w(1,0);
        for(int k=0;k<mid;k++,w=w*wn) {
          if(k%64==0) w={cos(2.0*k*PI/(mid<<1)),type*sin(2.0*k*PI/(mid<<1))};
          rts[k]=w;
        }
      }
		  for(int len=mid<<1,l=0;l<up-1;l+=len) {
			  for(int k=0;k<mid;k++) {
				  std::complex <double> x=A[l+k],y=rts[k]*A[l+mid+k];
				  A[l+k]=x+y;
				  A[l+mid+k]=x-y;
			  }
		  }
	  }
  }
  const long long BASE=10000,LENB=4;
  void int2048::standardize() {
    while(digits.size()&&digits[digits.size()-1]==0) {
      digits.pop_back();
    }
    if(digits.size()==0) flg=1;
  }
  int2048::int2048() {
    flg=1;
    digits={};
  }
  int2048::int2048(long long x) {
    digits.clear(); 
    if(x>=0) flg=1;
    else flg=-1;
    while(x) {
      digits.push_back(abs(x%BASE));
      x/=BASE;
    } 
    standardize();
  }
  int2048::int2048(const std::string &s) {
    digits.clear();
    if(s[0]=='-') {
      flg=-1;
      for(int i=(int)(s.size())-1;i>=1;i-=LENB) {
        long long x=0;
        for(int j=std::max(1ll,i-LENB+1);j<=i;j++) x=x*10+s[j]-'0';
        digits.push_back(x);
      }
    }
    else {
      flg=1;
      for(int i=(int)(s.size())-1;i>=0;i-=LENB) {
        long long x=0;
        for(int j=std::max(0ll,i-LENB+1);j<=i;j++) x=x*10+s[j]-'0';
        digits.push_back(x);
      }
    }
    standardize();
  }
  int2048::int2048(const int2048 &y) {
    flg=y.flg; digits=y.digits;
  }

  // 以下给定函数的形式参数类型仅供参考，可自行选择使用常量引用或者不使用引用
  // 如果需要，可以自行增加其他所需的函数
  // ===================================
  // Integer1
  // ===================================

  // 读入一个大整数
  void int2048::read(const std::string &s) {
    digits.clear();
    if(s[0]=='-') {
      flg=-1;
      for(int i=(int)(s.size())-1;i>=1;i-=LENB) {
        long long x=0;
        for(int j=std::max(1ll,i-LENB+1);j<=i;j++) x=x*10+s[j]-'0';
        digits.push_back(x);
      }
    }
    else {
      flg=1;
      for(int i=(int)(s.size())-1;i>=0;i-=LENB) {
        long long x=0;
        for(int j=std::max(0ll,i-LENB+1);j<=i;j++) x=x*10+s[j]-'0';
        digits.push_back(x);
      }
    }
    standardize();
  }
  // 输出储存的大整数，无需换行
  void int2048::PPT(long long x,bool type) {
    int NW=BASE/10;
    for(int i=LENB;i>=1;i--) {
      if(!type) break;
        if(x<NW) std::cout<<0;
        NW/=10;
    }
    if(x) std::cout<<x;
  }
  void int2048::print() {
    if(digits.size()==0) {
      std::cout<<0;
      return ;
    }
    if(flg==-1) std::cout<<'-';
    bool flg=0;
    for(int i=(int)(digits.size())-1;i>=0;i--) {
      PPT(digits[i],flg);
      flg=1;
    }
  }
  // 加上一个大整数
  int2048 &int2048::add(const int2048 &Extra) {
    if(digits.size()<=4&&Extra.digits.size()<=4) {
      *this=int2048(change_ll(*this)+change_ll(Extra));
      return *this;
    }
    int2048 extra=Extra;
    if(flg==extra.flg) {
      int len=std::max(extra.digits.size(),digits.size());
      while(digits.size()<=len) digits.push_back(0);
      for(int i=0;i<len;i++) {
        int nw=-1;
        if(i<(int)(extra.digits.size())) nw=extra.digits[i];
        else nw=0;
        digits[i]+=nw;
        if(digits[i]>=BASE) digits[i+1]+=digits[i]/BASE;
        digits[i]%=BASE;
      }
      standardize();
    }
    else {
      bool tp=1;
      if(digits.size()>extra.digits.size()) tp=1;
      else if(digits.size()<extra.digits.size()) tp=0;
      else {
        for(int i=(int)(digits.size())-1;i>=0;i--) {
          if(digits[i]==extra.digits[i]) continue;
          tp=(digits[i]>extra.digits[i]); break;
        }
      }
      int2048 EX;
      if(tp==0) {
        EX.flg=flg,EX.digits=digits;
        flg=extra.flg,digits=extra.digits;
      }
      else EX=extra;
      for(int i=0;i<(int)(digits.size());i++) {
        int nw=-1;
        if(i>=EX.digits.size()) {
          nw=0;
        }
        else nw=EX.digits[i];
        if(digits[i]>=nw) {
          digits[i]-=nw;
          continue;
        }
        digits[i]-=nw;
        digits[i]+=BASE;
        if(i==(int)(digits.size())-1) std::cerr<<"ERROR1"<<'\n';
        digits[i+1]--;
      }
      standardize();
    }
    return *this;
  }
  // 返回两个大整数之和
  int2048 add(int2048 a, const int2048 &b) {
    a.add(b);
    return a;
  }
  // 减去一个大整数
  int2048 &int2048::minus(const int2048 &extra) {
    int2048 c=extra; c.flg*=(-1);
    return add(c);
  }
  // 返回两个大整数之差
  int2048 minus(int2048 a, const int2048 &b) {
    a.minus(b);
    return a;
  }
  int2048 &int2048::naive_mul(const int2048& Extra) {
    int2048 extra=Extra;
    flg=flg*extra.flg;
    int len=digits.size()+extra.digits.size()+1;
    std::vector <long long> ans(len,0);
    for(int i=(int)(digits.size())-1;i>=0;i--) {
      if(digits[i]==0) continue;
      for(int j=(int)(extra.digits.size())-1;j>=0;j--) {
        if(extra.digits[j]==0) continue;
        ans[i+j]+=digits[i]*extra.digits[j];
      }
    }
    digits=std::move(ans);
    for(int i=0;i<(int)(digits.size())-1;i++) {
      digits[i+1]+=digits[i]/BASE;
      digits[i]%=BASE;
    }
    standardize();
    return (*this);
  }

  int2048 naive_mul(int2048 a, const int2048 &b) {
    a.naive_mul(b);
    return a;
  }
  int2048 &int2048::mul(const int2048& Extra) {
    if(digits.empty()||Extra.digits.empty()) {
      digits.clear(); flg=1;
      return *this;
    }
    if(Extra.digits.size()==1) {
      long long val=Extra.digits[0],carry=0;
      flg*=Extra.flg;
      for(auto &x:digits) {
        long long nw=x*val+carry;
        x=nw%BASE; carry=nw/BASE;
      }
      if(carry) digits.push_back(carry);
      standardize();
      return *this;
    }
    standardize(); int2048 cp=Extra; cp.standardize();
    if(std::min(digits.size(),cp.digits.size())<=200) {
      return naive_mul(Extra);
    }
    if(digits.size()==0||cp.digits.size()==0) {
      digits.clear();
      flg=1; return (*this);
    }
    std::vector <std::complex<double>> a{},b{};
    for(auto nw:digits) a.push_back({double(nw),0});
    for(auto nw:cp.digits) b.push_back({double(nw),0});
    int n=a.size()-1,m=b.size()-1,up=1;
    while(up<=n+m) up<<=1;
    while(a.size()<up) a.push_back({0,0});
    while(b.size()<up) b.push_back({0,0});
    fft(a,1),fft(b,1);
    for(int i=0;i<up;i++) a[i]*=b[i];
    fft(a,-1);
    std::vector <long long> vec;
    for(int i=0;i<up;i++) vec.push_back((long long)(a[i].real()/up+0.5));
    for(int i=1;i<=7;i++) vec.push_back(0);
    for(int i=0;i<(int)(vec.size())-1;i++) {
      vec[i+1]+=vec[i]/BASE;
      vec[i]%=BASE;
    }
    flg=flg*cp.flg; digits=std::move(vec); standardize();
    return (*this);
  }

  int2048 mul(int2048 a, const int2048 &b) {
    a.mul(b);
    return a;
  }
  bool int2048::geq(const std::vector <long long> &a,const std::vector <long long> &b) {
    int n=a.size(),m=b.size();
    while(n&&a[n-1]==0) n--;
    while(m&&b[m-1]==0) m--;
    if(n>m) return 1;
    else if(n<m) return 0;
    else {
      for(int i=n-1;i>=0;i--) {
        if(a[i]==b[i]) continue;
        return a[i]>b[i];
      }
    }
    return 1;
  }
  int2048 &int2048::naive_div(const int2048& Extra) {
    int2048 extra=Extra;
    int2048 ret(0);
    if(digits.size()==0) {
      flg=1,digits.clear();
      return (*this);
    }
    extra.standardize();
    if(!geq(digits,extra.digits)) {
      bool hh=(flg!=extra.flg);
      flg=1,digits.clear();
      if(hh) minus(1);
      return (*this);
    }
    int2048 give;
    int nt_extra=extra.flg; extra.flg=1;
    for(int i=(int)(digits.size())-1;i>=0;i--) {
      give.digits.insert(give.digits.begin(),digits[i]);
      give.standardize();
      int L=0,R=BASE-1,ans=-1;
      while(L<=R) {
        int M=(L+R)>>1;
        if(geq(give.digits,(int2048(M).naive_mul(extra).digits))) {
          ans=M,L=M+1;
        }
        else R=M-1;
      }
      ret.digits.push_back(ans);
      give.minus(int2048(ans).naive_mul(extra));
    }
    extra.flg=nt_extra;
    std::vector <long long> vivo{};
    for(int i=(int)ret.digits.size()-1;i>=0;i--) {
      vivo.push_back(ret.digits[i]);
    }
    give.standardize();
    if(!give.digits.size()||flg*extra.flg==1) {
      flg=extra.flg*flg;
      digits=vivo;
    }
    else {
      flg=extra.flg*flg;
      digits=vivo;
      minus(1);
    }
    standardize();
    return (*this);
  }
  int2048 naive_div(int2048 a, const int2048 &b) {
    a.naive_div(b);
    return a;
  }
  
  std::vector <int2048> int2048::REALDIV(int2048 a,int2048 b) {
    a.flg=b.flg=1;
    a.standardize(),b.standardize();
    if(a.digits.size()<=4&&b.digits.size()<=4) {
      long long x=change_ll(a),y=change_ll(b);
      return {int2048(x/y),int2048(x%y)};
    }
    if(b.digits.size()<=4) {
      long long y=change_ll(b);
      unsigned __int128 rem=0;
      for(int i=(int)a.digits.size()-1;i>=0;i--) {
        rem=rem*BASE+a.digits[i];
        a.digits[i]=rem/y;
        rem%=y;
      }
      a.standardize();
      return {a,int2048((long long)rem)};
    }
    if(!geq(a.digits,b.digits)) {
      if(a.flg==b.flg) {
        int2048 sh(0); a.flg=1;
        std::vector <int2048> ret;
        ret.push_back(sh); ret.push_back(a);
        return ret;
      }
      else {
        int2048 sh(-1); a.add(b); 
        std::vector <int2048> ret;
        ret.push_back(sh); ret.push_back(a);
        return ret;
      }
    }
    if(b.digits.size()<15) {
      int n=a.digits.size(),m=b.digits.size();
      long long fac=BASE/(b.digits.back()+1);
      a.mul(int2048(fac)); b.mul(int2048(fac));
      a.digits.resize(n+1,0);
      int2048 val(0);
      val.digits.assign(n-m+1,0);
      for(int j=n-m;j>=0;j--) {
        long long nw=a.digits[j+m]*BASE+a.digits[j+m-1];
        long long q=nw/b.digits[m-1],r=nw%b.digits[m-1];
        while(q>=BASE||q*b.digits[m-2]>r*BASE+a.digits[j+m-2]) {
          q--; r+=b.digits[m-1];
          if(r>=BASE) break;
        }
        long long carry=0;
        for(int i=0;i<m;i++) {
          long long cp=q*b.digits[i]+carry;
          carry=cp/BASE;
          a.digits[j+i]-=cp%BASE;
          if(a.digits[j+i]<0) {
            a.digits[j+i]+=BASE; carry++;
          }
        }
        a.digits[j+m]-=carry;
        if(a.digits[j+m]<0) {
          q--; carry=0;
          for(int i=0;i<m;i++) {
            long long cp=a.digits[j+i]+b.digits[i]+carry;
            a.digits[j+i]=cp%BASE; carry=cp/BASE;
          }
          a.digits[j+m]+=carry;
        }
        val.digits[j]=q;
      }
      a.digits.resize(m);
      long long rem=0;
      for(int i=m-1;i>=0;i--) {
        long long nw=rem*BASE+a.digits[i];
        a.digits[i]=nw/fac; rem=nw%fac;
      }
      val.standardize(); a.standardize();
      return {val,a};
    }
    int fac=BASE/(b.digits.back()+1),flg=false;
    if(fac>1) {
      a.naive_mul(fac),b.naive_mul(fac);
    }
    a.standardize(),b.standardize();
    if(b.digits.size()&1) {
      flg=true;
      a.digits.insert(a.digits.begin(),0),b.digits.insert(b.digits.begin(),0);
    }
    int len=b.digits.size()/2;
    std::vector <long long> give{},ret{};
    for(int i=((int)a.digits.size()-1)/len*len;i>=0;i-=len) {
      for(int j=std::min(i+len-1,(int)(a.digits.size())-1);j>=i;j--) {
      give.push_back(a.digits[j]);
      }
      std::vector <long long> rev_give;
      for(int j=(int)(give.size())-1;j>=0;j--) rev_give.push_back(give[j]);
      if(!geq(rev_give,b.digits)) {
        for(int j=1;j<=len;j++) ret.push_back(0);
        continue;
      }
      if(give.size()<=2*len) {
        for(int j=1;j<len;j++) ret.push_back(0);
        ret.push_back(1);
        int2048 ex(0); ex.digits=rev_give;
        ex.minus(b); ex.standardize();
        give.clear();
        for(int j=(int)(ex.digits.size())-1;j>=0;j--) give.push_back(ex.digits[j]);
        continue;
      }
      int2048 aa(0),bb(0); 
      for(int j=len;j<=std::min((int)rev_give.size(),3*len)-1;j++) aa.digits.push_back(rev_give[j]);
      for(int j=len;j<=2*len-1;j++) bb.digits.push_back(b.digits[j]);
      auto vec=REALDIV(aa,bb);
      int2048 val=vec[0]; 
      int2048 cp=val; cp.mul(b);
      while(geq(rev_give,cp.digits)) {
        cp.add(b); val.add(int2048(1));
      }
      while(true) {
        cp.minus(b); val.minus(int2048(1));
        if(geq(rev_give,cp.digits)) break;
      }
      val.standardize();
      for(int j=1;j<=len-(int)(val.digits.size());j++) ret.push_back(0);
      for(int j=(int)(val.digits.size())-1;j>=0;j--) ret.push_back(val.digits[j]);
      aa.digits=rev_give; aa.minus(cp);
      aa.standardize(); give.clear();
      for(int j=(int)(aa.digits.size())-1;j>=0;j--) give.push_back(aa.digits[j]);
    }
    int2048 val(0),res(0);
    for(int i=(int)(ret.size()-1);i>=0;i--) val.digits.push_back(ret[i]);
    for(int i=(int)(give.size())-1;i>=0;i--) res.digits.push_back(give[i]);
    val.standardize(),res.standardize();
    std::vector <int2048> RET;
    if(flg) {
      if(res.digits.size()) {
        res.digits.erase(res.digits.begin());
      }
    }
    if(fac>1) {
      long long rem=0;
      for(int i=(int)res.digits.size()-1;i>=0;i--) {
        long long nw=rem*BASE+res.digits[i];
        res.digits[i]=nw/fac; rem=nw%fac;
      }
    }
    res.standardize();
    RET.push_back(val),RET.push_back(res);
    return RET;
  }

  int2048& int2048::div(const int2048& Extra) {
    int tp=(flg==Extra.flg?1:-1); int2048 extra=Extra;
    flg=extra.flg=1;
    auto vec=REALDIV((*this),extra);
    (*this)=std::move(vec[0]); flg=tp;
    if(tp==-1&&vec[1].digits.size()) minus(1);
    standardize();
    return (*this);
  }

  int2048 div(int2048 a,const int2048& b) {
    a.div(b); return a;
  }
  // ===================================
  // Integer2
  // ===================================

  int2048 int2048::operator+() const {
    return (*this);
  }
  int2048 int2048::operator-() const {
    int2048 ret(*this);
    ret.flg*=(-1);
    return ret;
  }

  int2048 &int2048::operator+=(const int2048 &extra) {
    add(extra);
    return *this;
  }
  int2048 operator+(int2048 a, const int2048 &b) {
    a.add(b);
    return a;
  }

  int2048 &int2048::operator-=(const int2048 &extra) {
    return minus(extra);
  }
  int2048 operator-(int2048 a, const int2048 &b) {
    a.minus(b);
    return a;
  }

  int2048 &int2048::operator*=(const int2048 &extra) {
    mul(extra);
    return *this;
  }
  int2048 operator*(int2048 a, const int2048 &b) {
    a.mul(b);
    return a;
  }

  int2048 &int2048::operator/=(const int2048 &extra) {
    div(extra);
    return (*this);
  }
  int2048 operator/(int2048 a, const int2048 &b) {
    a.div(b);
    return a;
  }

  int2048 &int2048::operator%=(const int2048 &extra) {
    int tp=flg,bp=extra.flg;
    if(extra.digits.size()<=4) {
      long long y=change_ll(extra);
      if(y<0) y=-y;
      unsigned __int128 rem=0;
      for(int i=(int)digits.size()-1;i>=0;i--) rem=(rem*BASE+digits[i])%y;
      long long r=(long long)rem;
      if(tp!=bp&&r) r=y-r;
      *this=int2048(r*bp);
      return *this;
    }
    auto vec=REALDIV(*this,extra);
    int2048 res=std::move(vec[1]);
    if(tp!=bp&&!res.digits.empty()) {
      int2048 cp=extra; cp.flg=1;
      cp.minus(res); res=std::move(cp);
    }
    res.flg=bp; res.standardize();
    *this=std::move(res);
    return *this;
  }
  int2048 operator%(int2048 a, const int2048 &b) {
    a%=b;
    return a;
  }
  std::istream &operator>>(std::istream &in, int2048 &s) {
    std::string S;
    if(in>>S) s.read(S);
    return in;
  }
  std::ostream &operator<<(std::ostream &out, const int2048 &s) {
    if(s.digits.size()==0) {
      out<<0;
      return out;
    }
    if(s.flg==-1) {
      out<<'-';
    }
    bool flg=false,fst=true;
    for(int i=(int)(s.digits.size())-1;i>=0;i--) {
      if(s.digits[i]) flg=true;
      if(flg) {
        int NW=BASE/10,x=s.digits[i];
        for(int j=LENB;j>=1;j--) {
          if(fst) break;
          if(x<NW) out<<0;
          NW/=10;
        }
        fst=false;
        if(x) out<<x;
      }
    }
    return out;
  }

  int int2048::compare(const int2048 &y) const {
    int n=digits.size(),m=y.digits.size();
    while(n&&digits[n-1]==0) n--;
    while(m&&y.digits[m-1]==0) m--;
    int a=n?flg:1,b=m?y.flg:1;
    if(a!=b) return a<b?-1:1;
    if(n!=m) return (n<m?-1:1)*a;
    for(int i=n-1;i>=0;i--) {
      if(digits[i]!=y.digits[i]) return (digits[i]<y.digits[i]?-1:1)*a;
    }
    return 0;
  }
  bool operator==(const int2048 &a, const int2048 &b) {
    return a.compare(b)==0;
  }
  bool operator!=(const int2048 &a, const int2048 &b) {
    return a.compare(b)!=0;
  }
  bool operator<(const int2048 &a, const int2048 &b) {
    return a.compare(b)<0;
  }
  bool operator>(const int2048 &a, const int2048 &b) {
    return a.compare(b)>0;
  }
  bool operator<=(const int2048 &a, const int2048 &b) {
    return a.compare(b)<=0;
  }
  bool operator>=(const int2048 &a, const int2048 &b) {
    return a.compare(b)>=0;
  }
} // namespace sjtu