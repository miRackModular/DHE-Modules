require 'color'
require 'pathname'

require_relative 'dimensions'
require_relative 'shapes/box'
require_relative 'shapes/label'
require_relative 'shapes/line'
require_relative 'shapes/light'

require_relative 'controls/button'
require_relative 'controls/knob'
require_relative 'controls/pick_list'
require_relative 'controls/port'
require_relative 'controls/slide_switch'

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
    faceplate_background = Box.new(top: 0, left: 0, right: @width, bottom: MODULE_HEIGHT,
                                   fill: @background, stroke: @foreground, stroke_width: 1)
    module_label = Label.new(text: @name, size: :title, color: @foreground)
                        .translated(width / 2, MODULE_LABEL_INSET)
    author_label = Label.new(text: 'DHE', size: :title, color: @foreground, alignment: :below)
                        .translated(width / 2, MODULE_HEIGHT - MODULE_LABEL_INSET)
    @faceplate_shapes.prepend(faceplate_background, module_label, author_label)
    @faceplate_shape = CompositeShape.new(@faceplate_shapes)
    @image_shapes.prepend(@faceplate_shape)
    @image_shape = CompositeShape.new(@image_shapes)
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

  def line(x1:, y1:, x2:, y2:)
    @faceplate_shapes << Line.new(x1: x1, x2: x2, y1: y1, y2: y2, stroke: @foreground, width: STROKE_WIDTH)
  end

  def separator(y:)
    line(x1: 0, y1: y, x2: @width, y2: y)
  end

  def connector(left:, right:, y:)
    line(x1: left, x2: right, y1: y, y2: y)
  end

  def port(x:, y:, label: '')
    port = Port.new(metal_color: @background, shadow_color: @foreground)
    @control_shapes << port

    image_port = port.translated(x, y)
                     .padded(all: PADDING)
    @image_shapes << image_port

    faceplate_label = Label.new(text: label, color: @foreground, size: :small)
                           .translated(image_port.x, image_port.top)
    @faceplate_shapes << faceplate_label
  end

  def cv_port(x:, y:)
    port(x: x, y: y, label: 'CV')
  end

  def input_port(x:, y:, label: 'IN')
    port = Port.new(metal_color: @background, shadow_color: @foreground)
    @control_shapes << port

    image_port = port.translated(x, y)
                     .padded(all: PADDING)
    @image_shapes << image_port

    faceplate_label = Label.new(text: label, color: @foreground, size: :small)
                           .translated(image_port.x, image_port.top)
                           .padded(top: PADDING, right: 0.0, bottom: 0.0)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_port.right,
                            bottom: image_port.bottom, left: image_port.left,
                            stroke: @foreground, stroke_width: STROKE_WIDTH, fill: @background)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end

  def output_port(x:, y:, label: 'OUT')
    port = Port.new(metal_color: @background, shadow_color: @foreground)
    @control_shapes << port

    image_port = port.translated(x, y)
                     .padded(all: PADDING)
    @image_shapes << image_port

    faceplate_label = Label.new(text: label, color: @background, size: :small)
                           .translated(image_port.x, image_port.top)
                           .padded(top: PADDING, right: 0.0, bottom: 0.0)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_port.right,
                            bottom: image_port.bottom, left: image_port.left,
                            stroke: @foreground, stroke_width: STROKE_WIDTH, fill: @foreground)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
    @faceplate_shapes << faceplate_label
  end

  def knob(x:, y:, size:, label: nil, label_size:)
    knob = Knob.new(knob_color: @foreground, pointer_color: @background, size: size)
    @control_shapes << knob

    image_knob = knob.translated(x, y)
                     .padded(top: PADDING, right: 0.0, bottom: 0.0)
    @image_shapes << image_knob

    return if label.nil?

    faceplate_label = Label.new(text: label, color: @foreground, size: label_size)
                           .translated(image_knob.x, image_knob.top)
    @faceplate_shapes << faceplate_label
  end

  def small_knob(x:, y:, label: nil)
    knob(x: x, y: y, size: :small, label: label, label_size: :small)
  end

  def medium_knob(x:, y:, label: nil)
    knob(x: x, y: y, size: :medium, label: label, label_size: :small)
  end

  def large_knob(x:, y:, label: nil)
    knob(x: x, y: y, size: :large, label: label, label_size: :large)
  end

  def attenuverter(x:, y:)
    knob(x: x, y: y, size: :tiny, label: '- +', label_size: :large)
  end

  def toggle(x:, y:, labels:, selection:)
    toggles = (1..labels.size).map do |position|
      SlideSwitch::new(foreground: @foreground, background: @background, size: labels.size, position: position)
    end
    @control_shapes += toggles

    image_toggle = toggles[selection - 1].translated(x, y)
                                         .padded(all: PADDING)
    @image_shapes.append(image_toggle)

    low_label = Label.new(text: labels.first, color: @foreground, size: :small, alignment: :below)
                     .translated(image_toggle.x, image_toggle.bottom)
    high_label = Label.new(text: labels.last, color: @foreground, size: :small, alignment: :above)
                      .translated(image_toggle.x, image_toggle.top)
    @faceplate_shapes.append(low_label, high_label)

    if labels.size == 3
      mid_label = Label.new(text: labels[1], color: @foreground, size: :small, alignment: :right_of)
                       .translated(image_toggle.right, image_toggle.y)
      @faceplate_shapes << mid_label
    end
  end

  def polarity_toggle(x:, y:, selection: 1)
    toggle(x: x, y: y, labels: %w[BI UNI], selection: selection)
  end

  def duration_toggle(x:, y:)
    toggle(x: x, y: y, labels: %w[1 10 100], selection: 2)
  end

  def shape_toggle(x:, y:)
    toggle(x: x, y: y, labels: %w[J S], selection: 1)
  end

  def pick_list(x:, y:, name:, options:, selection: 1, width:, hidden: false)
    items = options.each_with_index.map do |option, index|
      PickList::Item.new(name: name, text: option, position: index + 1, text_color: @foreground, fill: @background, width: width)
    end

    image_item = items[selection - 1].translated(x, y)
                                     .padded(all: PADDING)

    menu = PickList::Menu.new(
      name: name, color: @foreground,
      width: image_item.width, height: image_item.height)

    @control_shapes += items
    @control_shapes << menu

    @image_shapes << image_item unless hidden
  end

  def button(x:, y:, label: nil, name: 'button')
    pressed = Button.new(name: name, button_color: @background, ring_color: @foreground, state: :pressed)
    released = Button.new(name: name, button_color: @foreground, ring_color: @foreground, state: :released)
    @control_shapes.append(pressed, released)

    image_button = released.translated(x, y)
                           .padded(all: PADDING)
    @image_shapes << image_button

    return if label.nil?

    faceplate_label = Label.new(text: label, color: @foreground, size: :small, alignment: :above)
                           .translated(image_button.x, image_button.top)
    @faceplate_shapes << faceplate_label
  end

  def input_button_port(x:, y:, label:)
    port = Port.new(metal_color: @background, shadow_color: @foreground)
    pressed_button = Button.new(button_color: @background, ring_color: @foreground, state: :pressed)
    released_button = Button.new(button_color: @foreground, ring_color: @foreground, state: :released)
    @control_shapes.append(port, pressed_button, released_button)

    image_port = port.translated(x, y)
                     .padded(all: PADDING)
    image_button = released_button.translated(image_port.right + released_button.right, image_port.y)
                                  .padded(all: PADDING, left: 0.0)
    @image_shapes.append(image_port, image_button)

    faceplate_label = Label.new(text: label, color: @foreground, size: :small)
                           .translated(image_port.x, image_port.top)
                           .padded(top: PADDING)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_button.right,
                            bottom: image_port.bottom, left: image_port.left,
                            stroke: @foreground, stroke_width: STROKE_WIDTH, fill: @background)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end

  def output_button_port(x:, y:, label:)
    port = Port.new(metal_color: @background, shadow_color: @foreground)
    pressed_button = Button.new(name: 'output-button', button_color: @foreground, ring_color: @background, state: :pressed)
    released_button = Button.new(name: 'output-button', button_color: @background, ring_color: @background, state: :released)
    @control_shapes.append(port, pressed_button, released_button)

    image_port = port.translated(x, y)
                     .padded(all: PADDING)
    image_button = released_button.translated(image_port.left + released_button.left, image_port.y)
                                  .padded(all: PADDING, right: 0.0)
    @image_shapes.append(image_port, image_button)

    faceplate_label = Label.new(text: label, color: @background, size: :small)
                           .translated(image_port.x, image_port.top)
                           .padded(top: PADDING)
    faceplate_box = Box.new(top: faceplate_label.top, right: image_port.right,
                            bottom: image_port.bottom, left: image_button.left,
                            stroke: @foreground, stroke_width: STROKE_WIDTH, fill: @foreground)
    @faceplate_shapes.append(faceplate_box, faceplate_label)
  end

  def label(x:, y:, text:, color: @foreground, size: :small, alignment: :right_of)
    @faceplate_shapes << Label.new(text: text, color: color, size: size, alignment: alignment)
                              .translated(x, y)
  end

  def light(x:, y:, color: @foreground)
    @faceplate_shapes << Light.new(color: color).translated(x, y)
  end
end