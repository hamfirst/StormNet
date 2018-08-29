<<<<<<< HEAD

#include "NetRandom.h"

// From http://www.cse.yorku.ca/~oz/marsaglia-rng.html

#define znew (z=36969*(z&65535)+(z>>16))
#define wnew (w=18000*(w&65535)+(w>>16))
#define MWC ((znew<<16)+wnew )
#define SHR3 (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
#define CONG (jcong=69069*jcong+1234567)
#define FIB ((b=a+b),(a=b-a))
#define KISS ((MWC^CONG)+SHR3)
#define LFIB4 (c++,t[c]=t[c]+t[UC(c+58)]+t[UC(c+119)]+t[UC(c+178)])
#define SWB (c++,bro=(x<y),t[c]=(x=t[UC(c+34)])-(y=t[UC(c+19)]+bro))
#define UNI (KISS*2.328306e-10)
#define VNI ((long) KISS)*4.656613e-10
#define UC (unsigned char)

using UL = unsigned long;

static UL z = 362436069, w = 521288629, jsr = 123456789, jcong = 380116160;
static UL a = 224466889, b = 7584631, t[256];

static UL x = 0, y = 0, bro; 
static unsigned char c = 0;

struct NetRandomInit
{
  NetRandomInit()
  {
    settable(12345, 65435, 34221, 12345, 9983651, 95746118);
  }

  void settable(UL i1, UL i2, UL i3, UL i4, UL i5, UL i6)
  {
    int i; z = i1; w = i2, jsr = i3; jcong = i4; a = i5; b = i6;
    for (i = 0; i<256; i = i + 1) t[i] = KISS;
  }
};

NetRandomInit s_RandomInit;

NetRandom::NetRandom() :
  m_State(0)
{

}

NetRandom::NetRandom(uint32_t seed) : 
  m_State(seed)
{

}

uint32_t NetRandom::GetRandom()
{
  auto z = m_State >> 16;
  auto w = m_State & 0x0000FFFF;

  uint8_t index = m_State >> 12;

  m_State = MWC ^ ~t[index];
  return m_State;
}

uint64_t NetRandom::GetRandom64()
{
  uint64_t r = GetRandom();
  r <<= 32;
  r |= GetRandom();
  return r;
}

uint32_t NetRandom::GetSeed() const
{
  return m_State;
}

=======

#include "NetRandom.h"

// From http://www.cse.yorku.ca/~oz/marsaglia-rng.html

#define znew (z=36969*(z&65535)+(z>>16))
#define wnew (w=18000*(w&65535)+(w>>16))
#define MWC ((znew<<16)+wnew )
#define SHR3 (jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5))
#define CONG (jcong=69069*jcong+1234567)
#define FIB ((b=a+b),(a=b-a))
#define KISS ((MWC^CONG)+SHR3)
#define LFIB4 (c++,t[c]=t[c]+t[UC(c+58)]+t[UC(c+119)]+t[UC(c+178)])
#define SWB (c++,bro=(x<y),t[c]=(x=t[UC(c+34)])-(y=t[UC(c+19)]+bro))
#define UNI (KISS*2.328306e-10)
#define VNI ((long) KISS)*4.656613e-10
#define UC (unsigned char)

using UL = unsigned long;

static UL z = 362436069, w = 521288629, jsr = 123456789, jcong = 380116160;
static UL a = 224466889, b = 7584631, t[256];

static UL x = 0, y = 0, bro; 
static unsigned char c = 0;

struct NetRandomInit
{
  NetRandomInit()
  {
    settable(12345, 65435, 34221, 12345, 9983651, 95746118);
  }

  void settable(UL i1, UL i2, UL i3, UL i4, UL i5, UL i6)
  {
    int i; z = i1; w = i2, jsr = i3; jcong = i4; a = i5; b = i6;
    for (i = 0; i<256; i = i + 1) t[i] = KISS;
  }
};

NetRandomInit s_RandomInit;

NetRandom::NetRandom() :
  m_State(0)
{

}

NetRandom::NetRandom(uint32_t seed) : 
  m_State(seed)
{

}

uint32_t NetRandom::GetRandom()
{
  auto z = m_State >> 16;
  auto w = m_State & 0x0000FFFF;

  uint8_t index = m_State >> 12;

  m_State = MWC ^ ~t[index];
  return m_State;
}

uint64_t NetRandom::GetRandom64()
{
  uint64_t r = GetRandom();
  r <<= 32;
  r |= GetRandom();
  return r;
}

uint32_t NetRandom::GetSeed() const
{
  return m_State;
}

>>>>>>> 82abf53995350c7136a2b761bbb2fb350eacf404
