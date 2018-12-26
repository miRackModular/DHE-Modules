require_relative 'shape'

module DHE
  class Button < RoundShape
    DIAMETER = 6.0

    def initialize(module_:, style: :normal)
      super(module_: module_, diameter: DIAMETER)
      @slug = 'button'
      @slug += '-reversed' if style == :reversed
      @ring_color = module_.foreground
      @center_color = module_.background
      @ring_color, @center_color = @center_color, @ring_color if style == :reversed
    end

    def svg_file(state:)
      position = state == :off ? 1 : 2
      path = module_.slug / "#{@slug}-#{position}"
      SvgFile.new(path: path, width: width, height: height) do |svg|
        draw_svg(svg: svg, state: state)
      end
    end

    def draw_svg(svg:, x: width / 2, y: height / 2, state: :off)
      center_color = state == :on ? @center_color : @ring_color
      stroke_width = DIAMETER / 6.0
      circle_diameter = DIAMETER - stroke_width
      circle_radius = circle_diameter / 2.0
      svg.circle(cx: x, cy: y, r: circle_radius, 'stroke-width' => stroke_width, fill: center_color, stroke: @ring_color)
    end
  end
end