
#include "graphics/GL.h"
//#include "graphics/shader.h"

#include "linked_list.h"
#include <stdio.h>

void printList(linked_list* list) {
    if (list->count == 0) {
        printf("list has no elements\n");
    } else {
        printf("list has %d elements: ", list->count);
        linked_list_node* n = list->first;
        while (n) {
            printf("%d, ", *((int*)n->item));
            n = n->next;
        }
        //printf("%d, ", *((int*)n->item));
        printf("\n");
    }
}

void testList() {

    linked_list* list = linked_list_ctor();
    {
        int a = 12;
        int b = 44;
        int c = 235;

        printList(list);
        linked_list_add(list, &a);
        printList(list);
        linked_list_add(list, &b);
        printList(list);
        linked_list_add(list, &c);
        a = 10;
        printList(list);
    }

    printList(list);


    printf("element at 0: %d", *((int*)linked_list_get(list, 0)));

}

int main() {

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1600, 900, "Test window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    while (!glfwWindowShouldClose(window)) {

        glClearColor(0, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}