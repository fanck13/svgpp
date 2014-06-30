#pragma once

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <agg_color_rgba.h>

typedef boost::variant<svgpp::tag::value::none, svgpp::tag::value::currentColor, agg::rgba8> SolidPaint;
struct IRIPaint
{
  IRIPaint(std::string const & fragment, boost::optional<SolidPaint> const & fallback = boost::optional<SolidPaint>())
    : fragment_(fragment)
    , fallback_(fallback)
  {}

  std::string fragment_;
  boost::optional<SolidPaint> fallback_;
};
typedef boost::variant<SolidPaint, IRIPaint> Paint;

struct InheritedStyle
{
  InheritedStyle()
    : color_(0, 0, 0)
    , stroke_width_(1.0)
    , stroke_opacity_(1.0)
    , fill_opacity_(1.0)
    , nonzero_fill_rule_(true)
    , line_cap_(agg::butt_cap)
    , line_join_(agg::miter_join)
    , miterlimit_(4.0)
    , fill_paint_(agg::rgba8(0, 0, 0))
    , stroke_paint_(svgpp::tag::value::none())
    , stroke_dashoffset_(0)
  {}

  agg::rgba8 color_;
  Paint fill_paint_, stroke_paint_;
  double stroke_width_;
  double stroke_opacity_, fill_opacity_;
  bool nonzero_fill_rule_;
  agg::line_cap_e line_cap_;
  agg::line_join_e line_join_;
  double miterlimit_;
  std::vector<double> stroke_dasharray_;
  double stroke_dashoffset_;
};

struct NoninheritedStyle
{
  NoninheritedStyle()
    : opacity_(1.0)
  {}

  double opacity_;
};

struct Style: 
  InheritedStyle,
  NoninheritedStyle
{
  struct inherit_tag {};

  Style()
  {}

  Style(Style const & src, inherit_tag)
    : InheritedStyle(src)
  {}
};

template<class AttributeTag>
class PaintContext
{
public:
  PaintContext(Paint & paint)
    : paint_(paint)
  {}

  void set(AttributeTag, svgpp::tag::value::none)
  { paint_ = svgpp::tag::value::none(); }

  void set(AttributeTag, svgpp::tag::value::currentColor)
  { paint_ = svgpp::tag::value::currentColor(); }

  void set(AttributeTag, agg::rgba8 color, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { paint_ = color; }

  template<class IRI>
  void set(AttributeTag tag, IRI const & iri)
  { 
    throw std::runtime_error("Non-local references aren't supported");
  }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment)
  { paint_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment))); }

  template<class IRI>
  void set(AttributeTag tag, IRI const &, svgpp::tag::value::none val)
  { set(tag, val); }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment, svgpp::tag::value::none val)
  { paint_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }

  template<class IRI>
  void set(AttributeTag tag, IRI const &, svgpp::tag::value::currentColor val)
  { set(tag, val); }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment, svgpp::tag::value::currentColor val)
  { paint_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }

  template<class IRI>
  void set(AttributeTag tag, IRI const &, agg::rgba8 val, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { set(tag, val); }

  template<class IRI>
  void set(AttributeTag tag, svgpp::tag::iri_fragment, IRI const & fragment, agg::rgba8 val, svgpp::tag::skip_icc_color = svgpp::tag::skip_icc_color())
  { paint_ = IRIPaint(std::string(boost::begin(fragment), boost::end(fragment)), boost::optional<SolidPaint>(val)); }
  
protected:
  Paint & paint_;
};

class Stylable:
  public PaintContext<svgpp::tag::attribute::stroke>,
  public PaintContext<svgpp::tag::attribute::fill>
{
public:
  typedef PaintContext<svgpp::tag::attribute::stroke> stroke_paint;
  typedef PaintContext<svgpp::tag::attribute::fill> fill_paint;

  Stylable()
    : stroke_paint(style_.stroke_paint_)
    , fill_paint(style_.fill_paint_)
  {}

  Stylable(Stylable const & src)
    : stroke_paint(style_.stroke_paint_)
    , fill_paint(style_.fill_paint_)
    , style_(src.style_, Style::inherit_tag())
  {}

  using stroke_paint::set;
  using fill_paint::set;

  void set(svgpp::tag::attribute::color, agg::rgba8 val)
  { style().color_ = val; }

  void set(svgpp::tag::attribute::stroke_width, double val)
  { style().stroke_width_ = val; }

  void set(svgpp::tag::attribute::stroke_opacity, double val)
  { style().stroke_opacity_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::fill_opacity, double val)
  { style().fill_opacity_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::opacity, double val)
  { style().opacity_ = std::min(1.0, std::max(0.0, val)); }

  void set(svgpp::tag::attribute::opacity, svgpp::tag::value::inherit)
  { style().opacity_ = parentStyle_.opacity_; }

  void set(svgpp::tag::attribute::fill_rule, svgpp::tag::value::nonzero)
  { style().nonzero_fill_rule_ = true; }

  void set(svgpp::tag::attribute::fill_rule, svgpp::tag::value::evenodd)
  { style().nonzero_fill_rule_ = false; }

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::butt)
  { style().line_cap_ = agg::butt_cap; }

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::round)
  { style().line_cap_ = agg::round_cap; }

  void set(svgpp::tag::attribute::stroke_linecap, svgpp::tag::value::square)
  { style().line_cap_ = agg::square_cap; }

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::miter)
  { style().line_join_ = agg::miter_join; }

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::round)
  { style().line_join_ = agg::round_join; }

  void set(svgpp::tag::attribute::stroke_linejoin, svgpp::tag::value::bevel)
  { style().line_join_ = agg::bevel_join; }

  void set(svgpp::tag::attribute::stroke_miterlimit, double val)
  { style().miterlimit_ = val; }

  void set(svgpp::tag::attribute::stroke_dasharray, svgpp::tag::value::none)
  { style().stroke_dasharray_.clear(); }

  template<class Range>
  void set(svgpp::tag::attribute::stroke_dasharray, Range const & range)
  { 
    style().stroke_dasharray_.assign(boost::begin(range), boost::end(range)); 
  }

  void set(svgpp::tag::attribute::stroke_dashoffset, double val)
  { style().stroke_dashoffset_ = val; }

  Style & style() { return style_; }
  Style const & style() const { return style_; }

private:
  Style style_;
  NoninheritedStyle parentStyle_;
};