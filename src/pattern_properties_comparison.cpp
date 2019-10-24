#include <wfc/ranges.h>

#include <wfc/overlapping_pattern_extraction.h>
#include <wfc/overlapping_types.h>

#include <algorithm>
#include <array>

namespace {

struct FlattenedPatternOccurence {
  Pattern pattern;

  int occurrence = 0;
};

bool operator==(const FlattenedPatternOccurence &left,
                const FlattenedPatternOccurence &right) {
  return (left.pattern == right.pattern && left.occurrence == right.occurrence);
}

std::array<FlattenedPatternOccurence, 8>
flattenPatternOccurrence(const PatternOccurrence &input) {
  std::array<FlattenedPatternOccurence, 8> toReturn;
  Dimension2D dimension{4, 2};
  int count = 0;
  auto consumerFcn = [&](const Index2D &index) {
    PatternTransformProperties transformProperties =
        denumerateTransformProperties(index);

    toReturn[count++] = {createPattern(input.pattern, transformProperties),
                         input.occurrence[index]};
  };
  runForDimension(dimension, consumerFcn);
  return toReturn;
}

bool patternsEquivalent(const PatternOccurrence &left,
                        const PatternOccurrence &right) {
  auto flattenedLeft = flattenPatternOccurrence(left);
  auto flattenedRight = flattenPatternOccurrence(right);

  // Don't have to check for size as usual for is_permutation as they're both
  // array size 8.
  return std::is_permutation(flattenedLeft.begin(), flattenedLeft.end(),
                             flattenedRight.begin());
}

bool enumeratedPatternsEquivalent(const EnumeratedPattern &left,
                                  const EnumeratedPattern &right) {
  PatternTransformProperties leftTransform =
      denumerateTransformProperties(left.enumeratedTransform);
  PatternTransformProperties rightTransform =
      denumerateTransformProperties(right.enumeratedTransform);

  Pattern leftTransformedPattern = createPattern(left.pattern, leftTransform);
  Pattern rightTransformedPattern =
      createPattern(right.pattern, rightTransform);

  return (leftTransformedPattern == rightTransformedPattern);
}

} // namespace

bool imagePropertiesEquivalent(const ImagePatternProperties &left,
                               const ImagePatternProperties &right) {
  // Pattern
  if (left.patterns.size() != right.patterns.size()) {
    return false;
  }

  bool patternsMatch =
      std::is_permutation(left.patterns.begin(), left.patterns.end(),
                          right.patterns.begin(), patternsEquivalent);
  return patternsMatch;
}

bool imageGridEquivalent(const ImagePatternProperties &left,
                         const ImagePatternProperties &right) {
  if (left.grid.size() != right.grid.size()) {
    return false;
  }

  Dimension2D dimension = left.grid.size();

  bool toReturn = true;

  auto functor = [&](const Index2D &index) {
    const PatternIdentifier &leftIdentifier = left.grid[index];
    const PatternIdentifier &rightIdentifier = right.grid[index];

    EnumeratedPattern enumeratedLeft = {
        left.patterns[leftIdentifier.patternIndex].pattern,
        leftIdentifier.enumeratedTransform};

    EnumeratedPattern enumeratedRight = {
        right.patterns[rightIdentifier.patternIndex].pattern,
        rightIdentifier.enumeratedTransform};

    bool matching =
        enumeratedPatternsEquivalent(enumeratedLeft, enumeratedRight);

    if (!matching) {
      toReturn = false;
    }

    // If any not matching - break out of the loop
    return !matching;
  };

  BreakRange::runForDimension(dimension, functor);

  return toReturn;
}
