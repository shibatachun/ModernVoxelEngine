#include "DataFormat.h"

glm::mat4 Node::localMatrix()
{
    return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
}

glm::mat4 Node::getMatrix()
{
    glm::mat4 m = localMatrix();
    Node* p = parent;
    while (p) {
        m = p->localMatrix() * m;
        p = p->parent;
    }
}
