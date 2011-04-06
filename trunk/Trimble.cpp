// Trimble implementation 
//
//
#include "Trimble.h"

Trimble::Trimble()
{
    m_num_chars = 0;
}
 
bool Trimble::encode(byte c)
{
    
    if (c == DLE)
    {    
	if (m_dle_seen)
        {
            m_read_buffer[m_num_chars++] = c;
            m_dle_seen = FALSE;
        }
        else if(c == ETX)
        {
	    if(m_read_buffer[0] = DLE)
	        parsePacket();  //complete packet RXed
	    else
		m_num_chars = 0;  //partial packet (just pitch it)
	}
	else
	{
	    m_dle_seen = TRUE; //we've now seen a DLE
	}
    }
    else
	if(m_dle_seen) 
	   m_read_buffer[m_num_chars++] = DLE;
	m_read_buffer[m_num_chars++] = c;
}
