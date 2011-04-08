// Trimble implementation 
//
//
#include "Trimble.h"

Trimble::Trimble()
{
   m_num_chars = 0;
	m_state = START;
}
 
bool Trimble::encode(byte c)
{
  switch (m_state)
  {
    case START:
      if (c == DLE)
        m_state = FRAMING_DLE;
     break;
   case FRAMING_DLE:
	  //mis-framed
	  if (c == DLE || c == ETX)
		 m_state = START;
	  else
	  {
		 state = DATA;
		 m_num_chars = 0;
		 m_read_buffer[m_num_chars++] = c;
	  }
	  break;
	case DATA:
	  //found data DLE
	  if (c == DLE)
		 m_state = DATA_DLE;
	  //add byte to packet
	  else
		 m_read_buffer[m_num_chars++] = c;
	  break;
	case DATA_DLE:
	  if (c == DLE)
	  {
		 m_read_buffer[m_num_chars++] = c;
		 m_state = DATA;
	  }
	  else if (c == ETX)
	  {
		 m_state = START;
		 parsePacket();   //Whoohoo the moment we've been waitin for
		 return TRUE;
	  }
	  //mis-framed
	  else
		 m_state = START;
	  break;
	default:
	  m_state = START;
	  break;
	}
	return FALSE;
}
