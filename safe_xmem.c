/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_xmem.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:17:47 by vzurera-          #+#    #+#             */
/*   Updated: 2025/12/04 23:27:16 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>

#define HASH_SIZE	1031

void	*__real_malloc(size_t size);
void	*__real_calloc(size_t nmemb, size_t size);
void	*__real_realloc(void *ptr, size_t size);
void	__real_free(void *ptr);
void	*hash_index(void *ptr);
void	**mem_find(void *ptr);
void	mem_delete(void *ptr, int just_node);

void	*__wrap_calloc(size_t nmemb, size_t size)
{
	void			*ptr;
	void			**hashtable;
	void			**mem;
	void			**new_mem;
	unsigned int	i;

	ptr = __real_calloc(nmemb, size);
	if (!ptr)
		return (NULL);
	hashtable = (void **)hash_index(NULL);
	if (!hashtable)
		return (NULL);
	mem = mem_find(ptr);
	if (mem)
		return (__real_free(*mem), *mem = ptr, ptr);
	i = (unsigned long)hash_index(ptr) % HASH_SIZE;
	new_mem = __real_malloc(sizeof(void *) * 2);
	if (!new_mem)
		return (__real_free(ptr), NULL);
	new_mem[0] = ptr;
	new_mem[1] = hashtable[i];
	hashtable[i] = new_mem;
	return (ptr);
}

void	*__wrap_realloc(void *ptr, size_t size)
{
	void			*oldptr;
	void			**hashtable;
	void			**mem;
	void			**new_mem;

	oldptr = ptr;
	ptr = __real_realloc(ptr, size);
	if (!ptr || oldptr == ptr)
		return (ptr);
	if (oldptr != ptr)
		mem_delete(oldptr, 1);
	hashtable = (void **)hash_index(NULL);
	if (!hashtable)
		return (NULL);
	mem = mem_find(ptr);
	if (mem)
		return (__real_free(*mem), *mem = ptr, ptr);
	new_mem = __real_malloc(sizeof(void *) * 2);
	if (!new_mem)
		return (__real_free(ptr), NULL);
	new_mem[0] = ptr;
	new_mem[1] = hashtable[(unsigned long)hash_index(ptr) % HASH_SIZE];
	hashtable[(unsigned long)hash_index(ptr) % HASH_SIZE] = new_mem;
	return (ptr);
}
