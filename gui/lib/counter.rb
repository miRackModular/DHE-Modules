require_relative 'control'

module DHE
  class Counter < RoundControl
    DIAMETER = 6.0

    def initialize(spec:)
      super(spec: spec, diameter: DIAMETER)
      @labels = spec[:labels]
      @position = (spec[:position] || 1) - 1
      @special = spec[:special]
    end

    def draw_foreground_svg(svg:, x:, y:, foreground:, background:)
      ring_color = foreground
      center_color = foreground
      stroke_width = DIAMETER / 6.0
      circle_diameter = DIAMETER - stroke_width
      circle_radius = circle_diameter / 2.0
      svg.circle(cx: x, cy: y, r: circle_radius, 'stroke-width' => stroke_width, fill: center_color, stroke: ring_color)
    end
  end
end