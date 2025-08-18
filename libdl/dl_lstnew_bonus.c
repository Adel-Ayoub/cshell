#include "libdl.h"

t_list *dl_lstnew(void *content)
{
    t_list *new;

    new = (t_list *)malloc(sizeof(t_list));
    if (!new)
        return (NULL);
    
    new->content = content;
    new->next = NULL;
    return (new);
}
