#include "Fontx.h"
#include "uni2sjis.h"

Fontx::Fontx()
{
  m_cFontx = 0;
  m_spaceX = 1;
  m_spaceY = 1;
}

void Fontx::resetFontx(void)
{
  m_cFontx = 0;
}
  
void Fontx::addFontx(const uint8_t *fontx)
{
  if(m_cFontx < FontxFileMax && fontx)
    m_aFontx[m_cFontx++] = fontx;
}

void Fontx::setFontx(const uint8_t *f0,const uint8_t *f1,const uint8_t *f2)
{
  resetFontx();
  addFontx(f0);
  addFontx(f1);
  addFontx(f2);
}

bool Fontx::getGlyph(uint16_t code , const uint8_t **pGlyph,
		     uint8_t *pw, uint8_t *ph)
{
  uint32_t sjis;

  if(code >= 0x100 && !uni2sjis(code, &sjis))
    return false;
  
  for(int i=0; i<m_cFontx; i++){
    const uint8_t *font = m_aFontx[i];
    bool is_ank = pgm_read_byte(&font[16]) == 0;

    uint8_t w = pgm_read_byte(&font[14]);
    uint8_t h = pgm_read_byte(&font[15]);
    uint32_t fsz = (w + 7) / 8 * h;

    if(code < 0x100){
      if(is_ank){
	if(pGlyph) *pGlyph = &font[17 + code * fsz];
	if(pw) *pw = w;
	if(ph) *ph = h;
	return true;
      }
    }
    else {
      unsigned int nc, bc, sb, eb;
      const uint8_t *cblk;

      cblk = &font[18]; nc = 0;
      bc = pgm_read_byte(&font[17]);
      while (bc--) {
	sb = pgm_read_byte(&cblk[0]) + pgm_read_byte(&cblk[1]) * 0x100;  
	eb = pgm_read_byte(&cblk[2]) + pgm_read_byte(&cblk[3]) * 0x100;
	if (sjis >= sb && sjis <= eb) {  
	  nc += sjis - sb;
	  if(pGlyph)
	    *pGlyph = &font[18 + 4 * pgm_read_byte(&font[17]) + nc * fsz];
	  if(pw) *pw = w;
	  if(ph) *ph = h;
	  return true;
	}
	nc += eb - sb + 1;    
	cblk += 4;            
      }
    }
  }
  return false;
}
