#include "./../include/HierarchicalRenderable.hpp"
#include "./../include/gl_helper.hpp"
#include "./../include/Viewer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

HierarchicalRenderable::HierarchicalRenderable(ShaderProgramPtr shaderProgram) :
    Renderable(shaderProgram),
    m_parent(nullptr),                      // default: no parent
    m_parentTransform(glm::mat4(1.0)),      // default: identity
    m_localTransform(glm::mat4(1.0))        // default: identity
{
}

HierarchicalRenderable::~HierarchicalRenderable()
{
}

const glm::mat4& HierarchicalRenderable::getParentTransform() const
{
    return m_parentTransform;
}

void HierarchicalRenderable::setParentTransform(const glm::mat4& parentTransform)
{
    m_parentTransform = parentTransform;
}

const glm::mat4& HierarchicalRenderable::getLocalTransform() const
{
    return m_localTransform;
}

void HierarchicalRenderable::setLocalTransform(const glm::mat4& localTransform)
{
    m_localTransform = localTransform;
}

void HierarchicalRenderable::updateModelMatrix()
{
    // TODO: compute the model matrix of the instance,
    // i.e. its position in world coordinates
    // Take into account the parent and local transformations

    m_model = computeTotalParentTransform() * m_localTransform;
}

glm::mat4 HierarchicalRenderable::computeTotalParentTransform() const
{
    // TODO: compute (recursively?) the total parent transform,
    // traversing the hierarchy upward until its root.
    //
    // if (m_parent) {
    //     // ...
    // } else {
    //     // ...
    // }

    if (m_parent) {
        return m_parent->computeTotalParentTransform() * m_parentTransform;
    } else {
        return m_parentTransform;
    }

    // return glm::mat4(1.0);  // return identity, to change...
}

void HierarchicalRenderable::setModelMatrix(const glm::mat4& model)
{
    setParentTransform(model);
    Renderable::setModelMatrix(model); // consistency
}


void HierarchicalRenderable::beforeDraw()
{
    //Each time m_localTransform is modified we need to update the model matrix of the instance.
    //Each time m_parentTransform is modified we need to udpate the model matrix of the instance and its children.
    //This could be implemented efficiently using a flag system to update the hierarchy whenever m_parentTransform is called.
    //However this is a simple implementation and we chose to pay the price of a brutal update before each draw.
    updateModelMatrix();
}

void HierarchicalRenderable::afterDraw()
{
    //After the instance has been drawn using do_draw(),
    //we loop over its children and draw them.
    //The subtlety is that these children can be drawn with different shaderProgram,
    //therefore we shall NOT forget to :
    //-Bind their respective shaderProgram
    //-Send projection and view matrix to the GPU
    //-Draw the object ;)
    //-Unbind their respective shaderProgram.
    for(size_t i=0; i<m_children.size(); ++i) {
        // this affectation here is a little hack we use to keep the source code simple.
        // As we go through the hierarchy for the drawing, we will need to have access to the camera, in order
        // to get the projection and the view matrices. The non root hierarchical renderables has not been added
        // to the viewer, thus they do not have the field m_viewer correctly setted. This is why we perform this
        // affectation here: we are then sure this field is up-to-date when a do_draw() method is called.
        m_children[i]->m_viewer = m_viewer;

        m_children[i]->bindShaderProgram();
        glcheck(glUniformMatrix4fv(m_children[i]->projectionLocation(), 1, GL_FALSE, glm::value_ptr(m_viewer->getCamera().projectionMatrix())));
        glcheck(glUniformMatrix4fv(m_children[i]->viewLocation(), 1, GL_FALSE, glm::value_ptr(m_viewer->getCamera().viewMatrix())));
        m_children[i]->draw();
        m_children[i]->unbindShaderProgram();
    }

}

void HierarchicalRenderable::afterAnimate(float dTime, float time)
{
    //After the instance has been animated using do_animate,
    //we loop over its children and animate them
    for (size_t i=0; i<m_children.size(); ++i)
        m_children[i]->animate(dTime, time);
}

void HierarchicalRenderable::addChild(HierarchicalRenderablePtr parent, HierarchicalRenderablePtr child)
{
    child->m_parent = parent;
    parent->m_children.push_back(child);
}

HierarchicalRenderablePtr HierarchicalRenderable::removeChild(HierarchicalRenderablePtr child) {
  // std::vector<HierarchicalRenderablePtr> &children = parent->getChildren();
  for (int i=0;i<m_children.size();i++) {
    if (m_children[i] == child) {
      m_children.erase(m_children.begin()+i);
      return child;
    }
  }
  return nullptr;
}

std::vector<HierarchicalRenderablePtr> & HierarchicalRenderable::getChildren()
{
    return m_children;
}
