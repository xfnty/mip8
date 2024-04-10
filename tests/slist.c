#include <util.h>

#include "test.h"


struct int_slist_node_t
{
    struct slist_node_t* _next;
    int data;
};

int main(int argc, char const *argv[]) {
    struct int_slist_node_t n3 = { NULL, 99 };
    struct int_slist_node_t n2 = { &n3, 99 };
    struct int_slist_node_t *head = malloc(sizeof(struct int_slist_node_t));
    *head = (struct int_slist_node_t){ &n2, 99 };

    slist_foreach(head, struct int_slist_node_t, node) {
        TEST_ASSERT_EQ(node->data, 99);
    }

    struct int_slist_node_t n4 = { NULL, 99 };
    slist_add(&head, &n4);
    int node_count = 0;
    slist_foreach(head, struct int_slist_node_t, node) {
        TEST_ASSERT_EQ(node->data, 99);
        node_count++;
    }
    TEST_ASSERT_EQ(node_count, 4);

    TEST_SUCCEED();
}
