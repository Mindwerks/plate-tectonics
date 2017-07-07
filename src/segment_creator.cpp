/******************************************************************************
 *  plate-tectonics, a plate tectonics simulation library
 *  Copyright (C) 2012-2013 Lauri Viitanen
 *  Copyright (C) 2014-2015 Federico Tomassetti, Bret Curtis
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see http://www.gnu.org/licenses/
 *****************************************************************************/

#include <memory>

#include "segment_creator.hpp"
#include "movement.hpp"
#include "segments.hpp"
#include "bounds.hpp"

MySegmentCreator::MySegmentCreator(std::shared_ptr<Bounds>  bounds, std::shared_ptr<ISegments> segments, HeightMap& map_)
: bounds(bounds), segments(segments), map(map_) {

}


uint32_t MySegmentCreator::calcDirection(const Platec::vec2ui& point, const uint32_t origin_index, const uint32_t ID) const
{
    if(bounds->isInLimits(point))
    {
        return ID;
    }

    if (hasLowerID(getLeftIndex(origin_index),ID)) {
        return segments->id(getLeftIndex(origin_index));
    } else if (hasLowerID(getRightIndex(origin_index),ID)) {
        return segments->id(getRightIndex(origin_index));
    } else if (hasLowerID(getTopIndex(origin_index),ID)) {
       return segments->id(getTopIndex(origin_index));
    } else if (hasLowerID(getBottomIndex(origin_index),ID)) {
      return segments->id(getBottomIndex(origin_index));
    }

    return ID;
}

Span MySegmentCreator::scanSpans(std::vector<Span>& spans_todo, std::vector<Span>& spans_done) const
{
    Span span;
    do // Find an unscanned span on this line.
    {
        span = spans_todo.back();
        spans_todo.pop_back();

        // Reduce any done spans from this span.
        for (const auto& tmpSpan : spans_done)
        {
            // Saved coordinates are AT the point
            // that was included last to the span.
            // That's why equalities matter.
            if (tmpSpan.inside(span.start ))
                span.start = getRightIndex(tmpSpan.end);

            if (tmpSpan.inside(span.end))
                span.end = getLeftIndex(tmpSpan.start);
        }

        if(span.end >= bounds->width())
        {
            span.start = std::numeric_limits<uint32_t>::max();
            return span;
        }
    } while (span.notValid() && spans_todo.empty());
    return span;
}

ContinentId MySegmentCreator::createSegment(const Platec::vec2ui& point, 
                                    const Dimension& worldDimension) 
{
    const uint32_t bounds_width = bounds->width();
    const uint32_t bounds_height = bounds->height();
    const uint32_t origin_index = bounds->index(point);
    const uint32_t ID = segments->size();

    if (segments->id(origin_index) < ID) {
        return segments->id(origin_index);
    }

    uint32_t nbour_id = calcDirection(point, origin_index, ID);

    if (nbour_id < ID)
    {
        segments->setId(origin_index, nbour_id);
        segments->getSegmentData(nbour_id).incArea();

        segments->getSegmentData(nbour_id).enlarge_to_contain(point);

        return nbour_id;
    }

    bool lines_processed = true;
    SegmentData pData = SegmentData(point,point, 0);
    std::vector<std::vector<Span>> spans_todo= std::vector<std::vector<Span>> (bounds_height);
    std::vector<std::vector<Span>> spans_done= std::vector<std::vector<Span>> (bounds_height);

    segments->setId(origin_index, ID);
    spans_todo[point.y()].emplace_back(point.x());
    
    uint32_t t = pData.getTop(),b = pData.getBottom(),r =pData.getRight(),l = pData.getLeft();

    while (lines_processed)
    {
        lines_processed = false;
        for (uint32_t line = 0; line < bounds_height; ++line)
        {
            
            if (spans_todo[line].empty())
                continue;

            Span span = scanSpans(spans_todo[line], spans_done[line]);

            if (span.notValid()) // Nothing to do here anymore...
                continue;
                        
            // Calculate line indices. Allow wrapping around map edges.
            const uint32_t row_above = (line == 0 ) ? bounds_height - 1 : line -1;
            const uint32_t row_below = (line < bounds_height - 1) ? line +1 : 0;
            const uint32_t line_here = line * bounds_width;

            const uint32_t line_above = row_above * bounds_width;
            const uint32_t line_below = row_below * bounds_width;

            // Extend the beginning of line.
            while (span.start != 0 && usablePoint(line_here+getLeftIndex(span.start),ID))
            {
                --span.start;
                segments->setId(line_here + span.start, ID);

                // Count volume of pixel...
            }

            // Extend the end of line.
            while (span.end < bounds_width - 1 
                    && usablePoint(line_here + getRightIndex(span.end),ID))
            {
                ++span.end;
                segments->setId(line_here + span.end, ID);

                // Count volume of pixel...
            }

            // Check if should wrap around left edge.
            if (bounds_width == worldDimension.getWidth() && span.start == 0 &&
                usablePoint(line_here+getLeftIndex(bounds_width),ID))
            {
                segments->setId(line_here + getLeftIndex(bounds_width), ID);
                spans_todo[line].emplace_back(getLeftIndex(bounds_width));

                // Count volume of pixel...
            }

            // Check if should wrap around right edge.
            if (bounds_width == worldDimension.getWidth() && span.end == bounds_width - 1 &&
                    usablePoint(line_here,ID))
            {
                segments->setId(line_here, ID);
                spans_todo[line].emplace_back(0);

                // Count volume of pixel...
            }

            pData.incArea(1 + span.end - span.start); // Update segment area counter.

            // Record any changes in extreme dimensions.
            t = std::min(line,t);
            b = std::max(line,b);
            l = std::min(span.start,l);
            r = std::max(span.end,r);

            if (line != 0 || bounds_height == worldDimension.getHeight()) {
                
               std::vector<Span> tmp = fillLineWithID(span,line_above, ID);
                
                spans_todo[row_above].insert(std::end(spans_todo[row_above]),
                            std::begin(tmp), std::end(tmp));

            }

            if (line < getLeftIndex(bounds_height) || bounds_height == worldDimension.getHeight()) {
                
                std::vector<Span> tmp = fillLineWithID(span,line_below, ID);
                
                spans_todo[row_below].insert(std::end(spans_todo[row_below]),
                            std::begin(tmp), std::end(tmp));
            }

            spans_done[line].emplace_back(span);
            lines_processed = true;
        }
    } 
    
    pData.setTop(t);
    pData.setBottom(b);
    pData.setLeft(l);
    pData.setRight(r);
    segments->add(pData);

    return ID;
}

const uint32_t MySegmentCreator::getBottomIndex(const int32_t originIndex) const {
    return originIndex + bounds->width();
}

const uint32_t MySegmentCreator::getLeftIndex(const int32_t originIndex) const {
    return originIndex-1;
}

const uint32_t MySegmentCreator::getRightIndex(const int32_t originIndex) const {
    return originIndex+1;
}

const uint32_t MySegmentCreator::getTopIndex(const int32_t originIndex) const {
    return originIndex - bounds->width();
}

const bool MySegmentCreator::hasLowerID(const uint32_t index, const ContinentId ID) const {
    //check if the value of the index is higher than CONT_BASE and
    //if ID is lower than the given ID
    return map.get(index) >= CONT_BASE && segments->id(index) < ID;
}

const bool MySegmentCreator::usablePoint(const uint32_t index, const ContinentId ID) const {
    return segments->id(index) > ID &&   map[index] >= CONT_BASE;
}

std::vector<Span> MySegmentCreator::fillLineWithID(const Span& span,const uint32_t line,
                                       const ContinentId ID ) 
{
    std::vector<Span> ret;
    ret.reserve(span.end-span.start *0.25); //reserve some Memory. for the worst case we have 4 realocation
    
    for (uint32_t j = span.start; j <= span.end; ++j) {
      if (usablePoint(line + j,ID))
      {
          uint32_t a = j;
          segments->setId(line + j, ID);

          // Count volume of pixel...
          while (++j < span.end &&
                  usablePoint(line + j,ID))
          {
              segments->setId(line + j, ID);

              // Count volume of pixel...
          }

          ret.emplace_back(a,getLeftIndex(j));
      }  
    }
    return ret;
    
    
}