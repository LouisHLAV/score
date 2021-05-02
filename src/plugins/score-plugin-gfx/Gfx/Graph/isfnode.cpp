#include "isfnode.hpp"
#include "renderedisfnode.hpp"

#include <score/tools/Debug.hpp>
#include <boost/algorithm/string/replace.hpp>


#include <Gfx/Qt5CompatPush> // clang-format: keep

namespace
{
struct input_size_vis
{
  int sz{};
  void operator()(const isf::float_input&) noexcept { sz += 4; }

  void operator()(const isf::long_input&) noexcept { sz += 4; }

  void operator()(const isf::event_input&) noexcept
  {
    sz += 4; // bool
  }

  void operator()(const isf::bool_input&) noexcept
  {
    sz += 4; // bool
  }

  void operator()(const isf::point2d_input&) noexcept
  {
    if (sz % 8 != 0)
      sz += 4;
    sz += 2 * 4;
  }

  void operator()(const isf::point3d_input&) noexcept
  {
    while (sz % 16 != 0)
    {
      sz += 4;
    }
    sz += 3 * 4;
  }

  void operator()(const isf::color_input&) noexcept
  {
    while (sz % 16 != 0)
    {
      sz += 4;
    }
    sz += 4 * 4;
  }

  void operator()(const isf::image_input&) noexcept
  {
    (*this)(isf::point2d_input{});
  }

  void operator()(const isf::audio_input&) noexcept { }

  void operator()(const isf::audioFFT_input&) noexcept { }
};

struct input_port_vis
{
  ISFNode& self;
  char* data{};
  int sz{};

  void operator()(const isf::float_input& in) noexcept
  {
    if (in.def != 0.)
      *reinterpret_cast<float*>(data) = in.def;
    else
      *reinterpret_cast<float*>(data) = (in.max - in.min) / 2.;
    self.input.push_back(new Port{&self, data, Types::Float, {}});
    data += 4;
    sz += 4;
  }

  void operator()(const isf::long_input& in) noexcept
  {
    *reinterpret_cast<int*>(data) = in.def;
    self.input.push_back(new Port{&self, data, Types::Int, {}});
    data += 4;
    sz += 4;
  }

  void operator()(const isf::event_input& in) noexcept
  {
    *reinterpret_cast<int*>(data) = 0;
    self.input.push_back(new Port{&self, data, Types::Int, {}});
    data += 4;
    sz += 4;
  }

  void operator()(const isf::bool_input& in) noexcept
  {
    *reinterpret_cast<int*>(data) = in.def;
    self.input.push_back(new Port{&self, data, Types::Int, {}});
    data += 4;
    sz += 4;
  }

  void operator()(const isf::point2d_input& in) noexcept
  {
    if (sz % 8 != 0)
    {
      sz += 4;
      data += 4;
    }

    const auto& arr = in.def.value_or(std::array<double, 2>{0.5, 0.5});
    *reinterpret_cast<float*>(data) = arr[0];
    *reinterpret_cast<float*>(data + 4) = arr[1];
    self.input.push_back(new Port{&self, data, Types::Vec2, {}});
    data += 2 * 4;
    sz += 2 * 4;
  }

  void operator()(const isf::point3d_input& in) noexcept
  {
    while (sz % 16 != 0)
    {
      sz += 4;
      data += 4;
    }
    const auto& arr = in.def.value_or(std::array<double, 3>{0.5, 0.5, 0.5});
    *reinterpret_cast<float*>(data) = arr[0];
    *reinterpret_cast<float*>(data + 4) = arr[1];
    *reinterpret_cast<float*>(data + 8) = arr[2];
    self.input.push_back(new Port{&self, data, Types::Vec3, {}});
    data += 3 * 4;
    sz += 3 * 4;
  }

  void operator()(const isf::color_input& in) noexcept
  {
    while (sz % 16 != 0)
    {
      sz += 4;
      data += 4;
    }
    const auto& arr
        = in.def.value_or(std::array<double, 4>{0.5, 0.5, 0.5, 0.5});
    *reinterpret_cast<float*>(data) = arr[0];
    *reinterpret_cast<float*>(data + 4) = arr[1];
    *reinterpret_cast<float*>(data + 8) = arr[2];
    *reinterpret_cast<float*>(data + 12) = arr[3];
    self.input.push_back(new Port{&self, data, Types::Vec4, {}});
    data += 4 * 4;
    sz += 4 * 4;
  }

  void operator()(const isf::image_input& in) noexcept
  {
    self.input.push_back(new Port{&self, {}, Types::Image, {}});

    // Also add the vec2 imgRect uniform:
    if (sz % 8 != 0)
    {
      sz += 4;
      data += 4;
    }

    *reinterpret_cast<float*>(data) = 640;
    *reinterpret_cast<float*>(data + 4) = 480;
    data += 2 * 4;
    sz += 2 * 4;
  }

  void operator()(const isf::audio_input& audio) noexcept
  {
    self.audio_textures.push_back({});
    auto& data = self.audio_textures.back();
    data.fixedSize = audio.max;
    self.input.push_back(new Port{&self, &data, Types::Audio, {}});
  }

  void operator()(const isf::audioFFT_input& audio) noexcept
  {
    self.audio_textures.push_back({});
    auto& data = self.audio_textures.back();
    data.fixedSize = audio.max;
    data.fft = true;
    self.input.push_back(new Port{&self, &data, Types::Audio, {}});
  }
};

}

ISFNode::ISFNode(
    const isf::descriptor& desc,
    const QShader& vert,
    const QShader& frag)
    : ISFNode{desc, vert, frag, &TexturedTriangle::instance()}
{
}

ISFNode::ISFNode(
    const isf::descriptor& desc,
    const QShader& vert,
    const QShader& frag,
    const Mesh* mesh)
    : m_mesh{mesh}
    , descriptor{desc}
{
  m_vertexS = vert;
  m_fragmentS = frag;

  // Compoute the size required for the materials
  input_size_vis sz_vis{};

  // Size of the inputs
  for (const isf::input& input : desc.inputs)
  {
    std::visit(sz_vis, input.data);
  }

  // Size of the pass textures
  for (int i = 0; i < desc.pass_targets.size(); i++)
    sz_vis(isf::point2d_input{});

  m_materialSize = sz_vis.sz;

  // Allocate the required memory
  // TODO : this must be per-renderer, as the texture sizes may depend on the renderer....
  m_materialData.reset(new char[m_materialSize]);
  std::fill_n(m_materialData.get(), m_materialSize, 0);
  char* cur = m_materialData.get();

  // Create ports pointing to the data used for the UBO
  input_port_vis visitor{*this, cur};
  for (const isf::input& input : desc.inputs)
    std::visit(visitor, input.data);

  // Handle the pass textures size uniforms
  {
    char* data = visitor.data;
    int sz = visitor.sz;
    for (int i = 0; i < desc.pass_targets.size(); i++)
    {
      if (sz % 8 != 0)
      {
        sz += 4;
        data += 4;
      }

      *reinterpret_cast<float*>(data) = 640;
      *reinterpret_cast<float*>(data + 4) = 480;
      data += 2 * 4;
      sz += 2 * 4;
    }
  }

  output.push_back(new Port{this, {}, Types::Image, {}});
}

ISFNode::~ISFNode() { }

const Mesh& ISFNode::mesh() const noexcept
{
  return *this->m_mesh;
}

QSize ISFNode::computeTextureSize(const isf::pass& pass, QSize origSize)
{
  QSize res = origSize;

  ossia::math_expression e;
  ossia::small_pod_vector<double, 16> data;

  // Note : reserve is super important here,
  // as the expression parser takes *references* to the
  // variables.
  data.reserve(2 + descriptor.inputs.size());

  e.add_constant("var_WIDTH", data.emplace_back(res.width()));
  e.add_constant("var_HEIGHT", data.emplace_back(res.height()));
  int port_k = 0;
  for (const isf::input& input : descriptor.inputs)
  {
    auto port = this->input[port_k];
    if (std::get_if<isf::float_input>(&input.data))
    {
      e.add_constant(
          "var_" + input.name, data.emplace_back(*(float*)port->value));
    }
    else if (std::get_if<isf::long_input>(&input.data))
    {
      e.add_constant(
          "var_" + input.name, data.emplace_back(*(int*)port->value));
    }

    port_k++;
  }

  if (auto expr = pass.width_expression; !expr.empty())
  {
    boost::algorithm::replace_all(expr, "$", "var_");
    e.register_symbol_table();
    bool ok = e.set_expression(expr);
    if (ok)
      res.setWidth(e.value());
    else
      qDebug() << e.error().c_str() << expr.c_str();
  }
  if (auto expr = pass.height_expression; !expr.empty())
  {
    boost::algorithm::replace_all(expr, "$", "var_");
    e.register_symbol_table();
    bool ok = e.set_expression(expr);
    if (ok)
      res.setHeight(e.value());
    else
      qDebug() << e.error().c_str() << expr.c_str();
  }

  return res;
}


score::gfx::NodeRenderer* ISFNode::createRenderer(Renderer& r) const noexcept
{
  /*
  switch(this->m_descriptor.passes.size())
  {
    case 0:
      SCORE_ABORT;
    case 1:
      if(!this->m_descriptor.passes[0].persistent)
        return new SinglePassISFNode{*this};
      break;
    default:
      break;
  }*/
  return new RenderedISFNode{*this};
}

RenderedISFNode::~RenderedISFNode() { }
#include <Gfx/Qt5CompatPop> // clang-format: keep
