// Trimble implementation 
//
//
#include "Trimble.h"

Trimble::Trimble()
{
   m_num_chars = 0;
	m_state = START;
}
 
bool Trimble::encode(byte input)
{
  switch (m_state)
  {
    case START:
      if (input == DLE)
        m_state = FRAMING_DLE;
      break;
    case FRAMING_DLE:
      //mis-framed
      if (input == DLE || input == ETX)
        m_state = START;
      else
      {
        m_state = DATA;
        m_num_chars = 0;
        m_read_buffer[m_num_chars++] = byte(input);
      }
      break;
    case DATA:
      //found data DLE
      if (input == DLE)
        m_state = DATA_DLE;
      //add byte to packet
      else
        m_read_buffer[m_num_chars++] = input;
      break;
    case DATA_DLE:
      if (input == DLE)
      {
        m_read_buffer[m_num_chars++] = input;
        m_state = DATA;
      }
      else if (input == ETX)
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
