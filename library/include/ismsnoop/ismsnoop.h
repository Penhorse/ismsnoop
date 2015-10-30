#pragma once

struct ISMSnoopInstrument;

extern "C"
{

ISMSnoopInstrument * ismsnoop_open(const char * path);
void ismsnoop_close(ISMSnoopInstrument * instrument);
void ismsnoop_get_panel_icon_size(ISMSnoopInstrument * instrument, int * width, int * height, int * depth);
void ismsnoop_get_panel_icon_bytes(ISMSnoopInstrument * instrument, char * dest);

}
