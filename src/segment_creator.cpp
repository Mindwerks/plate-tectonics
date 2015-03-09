#include "segment_creator.hpp"
#include "movement.hpp"
#include "segments.hpp"
#include "bounds.hpp"

uint32_t MySegmentCreator::calcDirection(uint32_t x, uint32_t y, const uint32_t origin_index, const uint32_t ID) const
{
    uint32_t canGoLeft  = x > 0          && map[origin_index - 1]     >= CONT_BASE;
    uint32_t canGoRight = x < _bounds.width() - 1  && map[origin_index+1]       >= CONT_BASE;
    uint32_t canGoUp    = y > 0          && map[origin_index - _bounds.width()] >= CONT_BASE;
    uint32_t canGoDown  = y < _bounds.height() - 1 && map[origin_index + _bounds.width()] >= CONT_BASE;
    uint32_t nbour_id = ID;

    // This point belongs to no segment yet.
    // However it might be a neighbour to some segment created earlier.
    // If such neighbour is found, associate this point with it.
    if (canGoLeft && _segments->id(origin_index - 1) < ID) {
        nbour_id = _segments->id(origin_index - 1);
    } else if (canGoRight && _segments->id(origin_index + 1) < ID) {
        nbour_id = _segments->id(origin_index + 1);
    } else if (canGoUp && _segments->id(origin_index - _bounds.width()) < ID) {
        nbour_id = _segments->id(origin_index - _bounds.width());
    } else if (canGoDown && _segments->id(origin_index + _bounds.width()) < ID) {
        nbour_id = _segments->id(origin_index + _bounds.width());
    }

    return nbour_id;
}

void MySegmentCreator::scanSpans(const uint32_t line, uint32_t& start, uint32_t& end,
    std::vector<uint32_t>* spans_todo, std::vector<uint32_t>* spans_done) const
{
    do // Find an unscanned span on this line.
    {
        end = spans_todo[line].back();
        spans_todo[line].pop_back();

        start = spans_todo[line].back();
        spans_todo[line].pop_back();

        // Reduce any done spans from this span.
        for (uint32_t j = 0; j < spans_done[line].size();
             j += 2)
        {
            // Saved coordinates are AT the point
            // that was included last to the span.
            // That's why equalities matter.

            if (start >= spans_done[line][j] &&
                start <= spans_done[line][j+1])
                start = spans_done[line][j+1] + 1;

            if (end >= spans_done[line][j] &&
                end <= spans_done[line][j+1])
                end = spans_done[line][j] - 1;
        }

        // Unsigned-ness hacking!
        // Required to fix the underflow of end - 1.
        start |= -(end >= _bounds.width());
        end -= (end >= _bounds.width());

    } while (start > end && spans_todo[line].size());
}

ContinentId MySegmentCreator::createSegment(uint32_t x, uint32_t y) const throw()
{
    const uint32_t origin_index = _bounds.index(x, y);
    const uint32_t ID = _segments->size();

    if (_segments->id(origin_index) < ID) {
        return _segments->id(origin_index);
    }

    uint32_t nbour_id = calcDirection(x, y, origin_index, ID);

    if (nbour_id < ID)
    {
        _segments->setId(origin_index, nbour_id);
        (*_segments)[nbour_id].incArea();

        (*_segments)[nbour_id].enlarge_to_contain(x, y);

        return nbour_id;
    }

    uint32_t lines_processed;
    Platec::Rectangle r = Platec::Rectangle(_worldDimension, x, x, y, y);

    SegmentData* pData = new SegmentData(r, 0);

    std::vector<uint32_t>* spans_todo = new std::vector<uint32_t>[_bounds.height()];
    std::vector<uint32_t>* spans_done = new std::vector<uint32_t>[_bounds.height()];

    _segments->setId(origin_index, ID);
    spans_todo[y].push_back(x);
    spans_todo[y].push_back(x);

    do
    {
      lines_processed = 0;
      for (uint32_t line = 0; line < _bounds.height(); ++line)
      {
        uint32_t start, end;

        if (spans_todo[line].size() == 0)
            continue;

        scanSpans(line, start, end, spans_todo, spans_done);

        if (start > end) // Nothing to do here anymore...
            continue;

        // Calculate line indices. Allow wrapping around map edges.
        const uint32_t row_above = ((line - 1) & -(line > 0)) |
            ((_bounds.height() - 1) & -(line == 0));
        const uint32_t row_below = (line + 1) & -(line < _bounds.height() - 1);
        const uint32_t line_here = line * _bounds.width();
        const uint32_t line_above = row_above * _bounds.width();
        const uint32_t line_below = row_below * _bounds.width();

        // Extend the beginning of line.
        while (start > 0 && _segments->id(line_here+start-1) > ID &&
            map[line_here+start-1] >= CONT_BASE)
        {
            --start;
            _segments->setId(line_here + start, ID);

            // Count volume of pixel...
        }

        // Extend the end of line.
        while (end < _bounds.width() - 1 &&
            _segments->id(line_here + end + 1) > ID &&
            map[line_here + end + 1] >= CONT_BASE)
        {
            ++end;
            _segments->setId(line_here + end, ID);

            // Count volume of pixel...
        }

        // Check if should wrap around left edge.
        if (_bounds.width() == _worldDimension.getWidth() && start == 0 &&
            _segments->id(line_here+_bounds.width()-1) > ID &&
            map[line_here+_bounds.width()-1] >= CONT_BASE)
        {
            _segments->setId(line_here + _bounds.width() - 1, ID);
            spans_todo[line].push_back(_bounds.width() - 1);
            spans_todo[line].push_back(_bounds.width() - 1);

            // Count volume of pixel...
        }

        // Check if should wrap around right edge.
        if (_bounds.width() == _worldDimension.getWidth() && end == _bounds.width() - 1 &&
            _segments->id(line_here+0) > ID &&
            map[line_here+0] >= CONT_BASE)
        {
            _segments->setId(line_here + 0, ID);
            spans_todo[line].push_back(0);
            spans_todo[line].push_back(0);

            // Count volume of pixel...
        }

        pData->incArea(1 + end - start); // Update segment area counter.

        // Record any changes in extreme dimensions.
        if (line < pData->getTop()) pData->setTop(line);
        if (line > pData->getBottom()) pData->setBottom(line);
        if (start < pData->getLeft()) pData->setLeft(start);
        if (end > pData->getRight()) pData->setRight(end);

        if (line > 0 || _bounds.height() == _worldDimension.getHeight()) {
            for (uint32_t j = start; j <= end; ++j)
              if (_segments->id(line_above + j) > ID &&
                  map[line_above + j] >= CONT_BASE)
              {
                uint32_t a = j;
                _segments->setId(line_above + a, ID);

                // Count volume of pixel...

                while (++j < _bounds.width() &&
                       _segments->id(line_above + j) > ID &&
                       map[line_above + j] >= CONT_BASE)
                {
                    _segments->setId(line_above + j, ID);

                    // Count volume of pixel...
                }

                uint32_t b = --j; // Last point is invalid.

                spans_todo[row_above].push_back(a);
                spans_todo[row_above].push_back(b);
                ++j; // Skip the last scanned point.
              }
        }

        if (line < _bounds.height() - 1 || _bounds.height() == _worldDimension.getHeight()) {
            for (uint32_t j = start; j <= end; ++j)
              if (_segments->id(line_below + j) > ID &&
                  map[line_below + j] >= CONT_BASE)
              {
                uint32_t a = j;
                _segments->setId(line_below + a, ID);

                // Count volume of pixel...

                while (++j < _bounds.width() &&
                       _segments->id(line_below + j) > ID &&
                       map[line_below + j] >= CONT_BASE)
                {
                    _segments->setId(line_below + j, ID);

                    // Count volume of pixel...
                }

                uint32_t b = --j; // Last point is invalid.

                spans_todo[row_below].push_back(a);
                spans_todo[row_below].push_back(b);
                ++j; // Skip the last scanned point.
              }
        }

        spans_done[line].push_back(start);
        spans_done[line].push_back(end);
        ++lines_processed;
      }
    } while (lines_processed > 0);

    delete[] spans_todo;
    delete[] spans_done;
    _segments->add(pData);

    return ID;
}
