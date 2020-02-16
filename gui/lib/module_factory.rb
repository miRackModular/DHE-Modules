require 'color'
require 'pathname'

require_relative 'dimensions'
require_relative 'shapes/box'
require_relative 'shapes/button'
require_relative 'shapes/faceplate'
require_relative 'shapes/knob'
require_relative 'shapes/label'
require_relative 'shapes/line'
require_relative 'shapes/light'
require_relative 'shapes/picklist'
require_relative 'shapes/port'
require_relative 'shapes/toggle'

class ModuleFactory
  MODULE_LABEL_INSET = 9.0

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
    @faceplate_shape = CompositeShape.new(shapes: @faceplate_shapes)
    @image_shapes.prepend(@faceplate_shape)
    @image_shape = CompositeShape.new(shapes: @image_shapes)
    self
  end

  def name(name)
    @name = name
  end

  def hp(hp)
    @width = hp2mm(hp)
  end

  def foreground(color)
    @foreground = "##{Color::HSL.new(*color).to_rgb.hex}"
  end

  def background(color)
    @background = "##{Color::HSL.new(*color).to_rgb.hex}"
  end

  def separator(y:)
    @faceplate_shapes << Line.new(color: @foreground, x1: 0, y1: y, x2: @width, y2: y)
  end

  def line(x1:, y1:, x2:, y2:)
    @faceplate_shapes << Line.new(color: @foreground, x1: x1, x2: x2, y1: y1, y2: y2)
  end

  def connector(left:, right:, y:)
    @faceplate_shapes << Line.new(color: @foreground, x1: left, x2: right, y1: y, y2: y)
  end

  def button(x:, y:, label: '', style: :normal)
    pressed = Button.new(foreground: @foreground, background: @background, style: style, state: :pressed)
    released = Button.new(foreground: @foreground, background: @background, style: style, state: :released)
    @control_shapes.append(pressed, released)

    image_button = released.translate(x, y)
    @image_shapes << image_button

    label_color = style == :normal ? @foreground : @background
    faceplate_label = Label.new(text: label, color: label_color, size: :small, alignment: :above)
                           .translate(image_button.x, image_button.top - PADDING)
    @faceplate_shapes << faceplate_label
  end

  def port(x:, y:, label: '')
    port = Port.new(foreground: @foreground, background: @background)
    @control_shapes << port

    image_port = port.translate(x, y)
    @image_shapes << image_port

    faceplate_label = Label.new(text: label, color: @foreground, size: :large)
                           .translate(image_port.x, image_port.top - PADDING)
    @faceplate_shapes << faceplate_label
  end

  def input_port(x:, y:, label: 'IN')
    port = Port.new(foreground: @foreground, background: @background)
    @control_shapes << port

    image_port = port.translate(x, y)
    @image_shapes << image_port

    faceplate_label = Label.new(text: label, color: @foreground, size: :small)
                           .translate(image_port.x, image_port.top - PADDING)
    faceplate_box = Box.new(top: faceplate_label.top - PADDING, right: image_port.right + PADDING,
                            bottom: image_port.bottom + PADDING, left: image_port.left - PADDING,
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
    faceplate_box = Box.new(top: faceplate_label.top - PADDING, right: image_port.right + PADDING,
                            bottom: image_port.bottom + PADDING, left: image_port.left - PADDING,
                            stroke: @foreground, fill: @foreground)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
    @faceplate_shapes << faceplate_label
  end

  def cv_port(x:, y:)
    port = Port.new(foreground: @foreground, background: @background)
    @control_shapes << port

    image_port = port.translate(x, y)
    @image_shapes << image_port

    faceplate_label = Label.new(text: 'CV', color: @foreground, size: :small)
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

  def small_knob(x:, y:, label: '')
    knob(x: x, y: y, size: :small, label: label, label_size: :small)
  end

  def medium_knob(x:, y:, label:)
    knob(x: x, y: y, size: :medium, label: label, label_size: :small)
  end

  def large_knob(x:, y:, label:)
    knob(x: x, y: y, size: :large, label: label, label_size: :large)
  end

  def attenuverter(x:, y:)
    knob(x: x, y: y, size: :tiny, label: '- +', label_size: :large)
  end

  def toggle(x:, y:, labels:, selection:)
    toggles = (1..labels.size).map do |position|
      Toggle::new(foreground: @foreground, background: @background, size: labels.size, position: position)
    end
    @control_shapes += toggles

    image_toggle = toggles[selection - 1].translate(x, y)
    @image_shapes.append(image_toggle)

    low_label = Label.new(text: labels.first, color: @foreground, size: :small, alignment: :below)
                     .translate(image_toggle.x, image_toggle.bottom + PADDING)
    high_label = Label.new(text: labels.last, color: @foreground, size: :small, alignment: :above)
                      .translate(image_toggle.x, image_toggle.top - PADDING)
    @faceplate_shapes.append(low_label, high_label)

    if labels.size == 3
      mid_label = Label.new(text: labels[1], color: @foreground, size: :small, alignment: :right_of)
                       .translate(image_toggle.right + PADDING, image_toggle.y)
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

  def picklist(x:, y:, name:, options:, selection: 1, width:, hidden: false)
    selected_options = options.each_with_index.map do |option, index|
      Picklist::Option.new(
        name: name,
        text: option,
        position: index + 1,
        text_color_on: @foreground, text_color_off: @background,
        width: width,
        selected: true)
    end
    unselected_options = options.each_with_index.map do |option, index|
      Picklist::Option.new(
        name: name,
        text: option,
        position: index + 1,
        text_color_on: @foreground, text_color_off: @background,
        width: width,
        selected: false)
    end
    @control_shapes += selected_options
    @control_shapes += unselected_options
    @control_shapes << Picklist::Menu.new(
      name: name,
      border: @foreground, fill: @background,
      width: width, height: selected_options[0].height * options.size)

    default_option = selected_options[selection - 1].translate(x, y)

    @image_shapes << default_option unless hidden
  end

  def input_button_port(x:, y:, label:)
    port = Port.new(foreground: @foreground, background: @background)
    pressed_button = Button.new(foreground: @foreground, background: @background, state: :pressed)
    released_button = Button.new(foreground: @foreground, background: @background, state: :released)
    @control_shapes.append(port, pressed_button, released_button)

    image_port = port.translate(x, y)
    image_button = released_button.translate(image_port.right + PADDING + released_button.radius, image_port.y)
    @image_shapes.append(image_port, image_button)

    faceplate_label = Label.new(text: label, color: @foreground, size: :small)
                           .translate(image_port.x, image_port.top - PADDING)
    faceplate_box = Box.new(top: faceplate_label.top - PADDING, right: image_button.right + PADDING,
                            bottom: image_port.bottom + PADDING, left: image_port.left - PADDING,
                            stroke: @foreground, fill: @background)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end

  def output_button_port(x:, y:, label:)
    port = Port.new(foreground: @foreground, background: @background)
    pressed_button = Button.new(foreground: @foreground, background: @background, state: :pressed, style: :reversed)
    released_button = Button.new(foreground: @foreground, background: @background, state: :released, style: :reversed)
    @control_shapes.append(port, pressed_button, released_button)

    image_port = port.translate(x, y)
    image_button = released_button.translate(image_port.left - PADDING - released_button.radius, image_port.y)
    @image_shapes.append(image_port, image_button)

    faceplate_label = Label.new(text: label, color: @background, size: :small)
                           .translate(image_port.x, image_port.top - PADDING)
    faceplate_box = Box.new(top: faceplate_label.top - PADDING, right: image_port.right + PADDING,
                            bottom: image_port.bottom + PADDING, left: image_button.left - PADDING,
                            stroke: @foreground, fill: @foreground)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end

  def label(x:, y:, text:, color: @foreground, size: :small, alignment: :right_of)
    @faceplate_shapes << Label.new(text: text, color: color, size: size, alignment: alignment)
                              .translate(x, y)
  end

  def light(x:, y:, color: @foreground)
    @faceplate_shapes << Light.new(color: color).translate(x, y)
  end
end