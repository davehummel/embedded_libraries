#ifndef DH_BASE_LOGGER_H__
#define DH_BASE_LOGGER_H__
#include <stdint.h>
#include "Print.h"

class Logger : public Print
{
  public:
    Logger(uint16_t m_bufferSize = 256) : bufferSize(m_bufferSize)
    {
        buffer = new uint8_t[bufferSize];
        clear();
    }

    char *getText()
    {
        buffer[errBufCount] = '\0';
        return buffer;
    }

    void clear()
    {
        buffer[0] = '\0';
        bufferIndex = 0;
    }

    void finished()
    {
        if (handler != NULL){
            handler(getText());
        }
        clear();
    }

    virtual size_t write(uint8_t)
    {
        if (bufferIndex == bufferSize - 1)
        {
            return 0;
        }
        buffer[bufferIndex] = ch;
        bufferIndex++;
        return 1;
    }

    void setMessageHandler( void (*m_handler)(uint8_t *){
        handler = m_handler;
    }

  private:

    uint16_t bufferIndex;
    const uint16_t bufferSize;
    uint8_t *buffer;
    void (*handler)(uint8_t *) = NULL;
}