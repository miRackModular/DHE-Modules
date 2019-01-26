require 'color'
require 'pathname'

require_relative 'dimensions'
require_relative 'shapes/box'
require_relative 'shapes/button'
require_relative 'shapes/faceplate'
require_relative 'shapes/knob'
require_relative 'shapes/label'
require_relative 'shapes/line'
require_relative 'shapes/port'
require_relative 'shapes/toggle'

class ModuleFactory
  MM_PER_HP = 5.08
  MODULE_HEIGHT = 128.5
  MODULE_HEIGHT_PX = MODULE_HEIGHT * PX_PER_MM
  MODULE_LABEL_INSET = 9.0
  PADDING = 1.0

  attr_reader :source_file, :width, :slug, :faceplate_shape, :image_shape, :control_shapes

  def initialize(source_file)
    @source_file = Pathname(source_file)
    @slug = Pathname(source_file.to_s.pathmap('%n'))
    @faceplate_shapes = []
    @image_shapes = []
    @control_shapes = []
  end

  def build
    instance_eval(@source_file.read, @source_file.to_s)
    faceplate = Faceplate.new(top: 0, right: @width, bottom: MODULE_HEIGHT, left: 0, stroke: @foreground, fill: @background)
    module_label = Label.new(text: @name, size: :title, color: @foreground)
                       .translate(width / 2, MODULE_LABEL_INSET)
    author_label = Label.new(text: 'DHE', size: :title, color: @foreground, alignment: :below)
                       .translate(width / 2, MODULE_HEIGHT - MODULE_LABEL_INSET)
    @faceplate_shapes.prepend(faceplate, module_label, author_label)
    @faceplate_shape ||= CompositeShape.new(shapes: @faceplate_shapes)
    @image_shapes.prepend(@faceplate_shape)
    @image_shape ||= CompositeShape.new(shapes: @image_shapes)
    self
  end


  def name(name)
    @name = name
  end

  def hp(hp)
    @width = hp * MM_PER_HP
  end

  def foreground(color)
    @foreground = "##{Color::HSL.new(*color).to_rgb.hex}"
  end

  def background(color)
    @background = "##{Color::HSL.new(*color).to_rgb.hex}"
  end

  def separator(y:)
    @faceplate_shapes << Line.new(color: @foreground, left: 0, right: @width, y: y)
  end

  def connector(left:, right:, y:)
    @faceplate_shapes << Line.new(color: @foreground, left: left, right: right, y: y)
  end

  def button(x:, y:, label: nil, style: :normal)
    stroke = style == :normal ? @foreground : @background
    fill = style == :normal ? @background : @foreground

    pressed = Button.new(stroke: stroke, fill: fill, style: style)
    released = Button.new(stroke: stroke, fill: stroke, style: style)
    @control_shapes.append(pressed, released)

    image_button = released.translate(x, y)
    @image_shapes << image_button

    faceplate_label = Label.new(text: label, color: stroke, size: :small, alignment: :above)
                          .translate(image_button.x, image_button.top - PADDING)
    @faceplate_shapes << faceplate_label
  end

  def input_port(x:, y:, label: 'IN')
    port = Port.new(foreground: @foreground, background: @background)
    @control_shapes << port

    image_port = port.translate(x, y)
    @image_shapes << image_port

    faceplate_label = Label.new(text: label, color: @foreground, size: :small)
                          .translate(image_port.x, image_port.top - PADDING)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_port.right, bottom: image_port.bottom, left: image_port.left,
                            stroke: @foreground, fill: @background)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end

  def output_port(x:, y:, label: 'OUT')
    port = Port.new(foreground: @foreground, background: @background)
    @control_shapes << port

    image_port = port.translate(x, y)
    @image_shapes << image_port

    faceplate_label = Label.new(text: label, color: @background, size: :small)
                          .translate(image_port.x, image_port.top - PADDING)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_port.right, bottom: image_port.bottom, left: image_port.left,
                            stroke: @foreground, fill: @foreground)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
    @faceplate_shapes << faceplate_label
  end

  def cv_port(x:, y:)
    port = Port.new(foreground: @foreground, background: @background)
    @control_shapes << port

    image_port = port.translate(x, y)
    @image_shapes << image_port

    faceplate_label = Label.new(text: 'cv', color: @foreground, size: :small)
                          .translate(image_port.x, image_port.top - PADDING)
    @faceplate_shapes << faceplate_label
  end

  def knob(x:, y:, size:, label:, label_size:)
    knob = Knob.new(knob_color: @foreground, pointer_color: @background, size: size)
    @control_shapes << knob

    image_knob = knob.translate(x, y)
    @image_shapes << image_knob

    faceplate_label = Label.new(text: label, color: @foreground, size: label_size)
                          .translate(image_knob.x, image_knob.top - PADDING)
    @faceplate_shapes << faceplate_label
  end

  def small_knob(x:, y:, label:)
    knob(x: x, y: y, size: :small, label: label, label_size: :small)
  end

  def medium_knob(x:, y:, label:)
    knob(x: x, y: y, size: :medium, label: label, label_size: :small)
  end

  def large_knob(x:, y:, label:)
    knob(x: x, y: y, size: :large, label: label, label_size: :large)
  end

  def attenuverter(x:, y:)
    knob(x: x, y: y, size: :tiny, label: '<tspan font-size="larger">-&#160;&#160;+</tspan>', label_size: :large)
  end

  def toggle(x:, y:, labels:, selection:)
    housing = Toggle::Housing.new(foreground: @foreground, background: @background, size: labels.size)
    @control_shapes.append(housing)

    levers = (1..labels.size).map do |position|
      Toggle::Lever.new(foreground: @foreground, background: @background, size: labels.size, position: position)
    end
    @control_shapes += levers

    image_housing = housing.translate(x, y)
    image_lever = levers[selection - 1].translate(x, y)
    @image_shapes.append(image_housing, image_lever)

    low_label = Label.new(text: labels.first, color: @foreground, size: :small, alignment: :below)
                    .translate(image_housing.x, image_housing.bottom + PADDING)
    high_label = Label.new(text: labels.last, color: @foreground, size: :small, alignment: :above)
                     .translate(image_housing.x, image_housing.top - PADDING)
    @faceplate_shapes.append(low_label, high_label)

    if labels.size == 3
      mid_label = Label.new(text: labels[1], color: @foreground, size: :small, alignment: :right_of)
                      .translate(image_housing.right + PADDING, image_housing.y)
      @faceplate_shapes << mid_label
    end
  end

  def polarity_toggle(x:, y:, selection: 1)
    toggle(x: x, y: y, labels: %w(BI UNI), selection: selection)
  end

  def duration_toggle(x:, y:)
    toggle(x: x, y: y, labels: %w(1 10 100), selection: 2)
  end

  def shape_toggle(x:, y:)
    toggle(x: x, y: y, labels: %w(J S), selection: 1)
  end

  def stepper(x:, y:, name:, labels:, selection: 1, hidden: false)
    stepper_button = Button.new(stroke: @foreground, fill: @foreground)
    stepper_labels = labels.map do |label|
      Label.new(text: label, size: :small, color: @foreground, alignment: :above)
          .translate(stepper_button.x, stepper_button.top - PADDING)
    end
    steppers = stepper_labels.map do |label|
      CompositeShape.new(shapes: [stepper_button, label])
    end
    @control_shapes += steppers

    @image_shapes << steppers[selection - 1].translate(x, y) unless hidden
  end

  def input_button_port(x:, y:, label:)
    port = Port.new(foreground: @foreground, background: @background)
    pressed_button = Button.new(stroke: @foreground, fill: @background)
    released_button = Button.new(stroke: @foreground, fill: @foreground)
    @control_shapes.append(port, pressed_button, released_button)

    image_port = port.translate(x, y)
    image_button = released_button.translate(port.x + port.radius + PADDING + released_button.radius, port.y)
    @image_shapes.append(image_port, image_button)

    faceplate_label = Label.new(text: label, color: @foreground, size: :small)
                          .translate(image_port.x, image_port.top - PADDING)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_button.right,
                            bottom: image_port.bottom, left: image_port.left,
                            stroke: @foreground, fill: @background)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end

  def output_button_port(x:, y:, label:)
    port = Port.new(foreground: @foreground, background: @background)
    pressed_button = Button.new(stroke: @background, fill: @foreground)
    released_button = Button.new(stroke: @background, fill: @background)
    @control_shapes.append(port, pressed_button, released_button)

    image_port = port.translate(x, y)
    image_button = released_button.translate(port.x - port.radius - PADDING - released_button.radius, port.y)
    @image_shapes.append(image_port, image_button)

    faceplate_label = Label.new(text: label, color: @background, size: :small)
                          .translate(image_port.x, image_port.top - PADDING)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_button.right,
                            bottom: image_port.bottom, left: image_port.left,
                            stroke: @foreground, fill: @foreground)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end
end