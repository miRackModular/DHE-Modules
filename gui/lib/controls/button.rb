require_relative '../control'

module DHE
  class Button < RoundControl
    DIAMETER = 6.0

    def initialize(faceplate:, x:, y:, style: :normal)
      super(faceplate: faceplate, x: x, y: y, diameter: DIAMETER)
      @slug = 'button'
      @slug += '-reversed' if style == :reversed
      @ring_color = faceplate.foreground
      @center_color = faceplate.background
      @ring_color, @center_color = @center_color, @ring_color if style == :reversed
    end

    def draw(svg:, x: @x, y: @y, state: :off)
      center_color = state == :on ? @center_color : @ring_color
      stroke_width = DIAMETER / 6.0
      circle_diameter = DIAMETER - stroke_width
      circle_radius = circle_diameter / 2.0
      svg.circle(cx: x, cy: y, r: circle_radius, 'stroke-width' => stroke_width, fill: center_color, stroke: @ring_color)
    end

    def svg_files
      [:on, :off].map {|state| svg_file(state: state)}
    end

    def svg_file(state:)
      position = state == :off ? 1 : 2
      path = faceplate.slug / "#{@slug}-#{position}"
      SvgFile.new(path: path, width: width, height: height) do |svg|
        draw(svg: svg, state: state, x: @width / 2.0, y: @height / 2.0)
      end
    end
  end
end