#include "../include/yds_time_tag_data.h"

#include <stdlib.h>

ysTimeTagData::ysTimeTagData() {
    Clear();
}

ysTimeTagData::~ysTimeTagData() {
    /* void */
}

void ysTimeTagData::Clear() {
    m_timeTagCount = 0;
    m_timeTags.Clear();
}
