/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_mem.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 23:17:43 by vzurera-          #+#    #+#             */
/*   Updated: 2025/12/04 23:26:29 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdint.h>

#define HASH_SIZE	1031

void	*__real_malloc(size_t size);
void	__real_free(void *ptr);

void	*hash_index(void *ptr)
{
	static void		*hashtable[HASH_SIZE];
	unsigned long	addr;

	if (ptr == NULL)
		return (hashtable);
	addr = (unsigned long)ptr;
	addr = ~addr + (addr << 15);
	addr ^= (addr >> 12);
	addr += (addr << 2);
	addr ^= (addr >> 4);
	addr *= 2057;
	addr ^= (addr >> 16);
	return ((void *)(addr % HASH_SIZE));
}

void	**mem_find(void *ptr)
{
	void	**hashtable;
	void	**mem;

	hashtable = (void **)hash_index(NULL);
	if (!hashtable || !ptr)
		return (NULL);
	mem = (void **)hashtable[(uintptr_t)hash_index(ptr) % HASH_SIZE];
	while (mem)
	{
		if (mem[0] == ptr)
			return (mem);
		mem = mem[1];
	}
	return (NULL);
}

void	mem_delete(void *ptr, int just_node)
{
	void			**hashtable;
	void			**mem;
	void			**prev;

	hashtable = (void **)hash_index(NULL);
	if (!hashtable || !ptr)
		return ;
	mem = (void **)hashtable[(uintptr_t)hash_index(ptr) % HASH_SIZE];
	prev = NULL;
	while (mem)
	{
		if (mem[0] == ptr)
		{
			if (prev)
				prev[1] = mem[1];
			else
				hashtable[(unsigned long)hash_index(ptr) % HASH_SIZE] = mem[1];
			if (!just_node)
				__real_free(mem[0]);
			__real_free(mem);
			return ;
		}
		prev = mem;
		mem = mem[1];
	}
}

void	*__wrap_malloc(size_t size)
{
	void			*ptr;
	void			**hashtable;
	void			**mem;
	void			**new_mem;

	ptr = __real_malloc(size);
	if (!ptr)
		return (NULL);
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

void	__wrap_free(void *ptr)
{
	void			**hashtable;
	void			**mem;
	void			*next;
	unsigned int	i;

	if (ptr == (void *)-42)
	{
		i = -1;
		hashtable = (void **)hash_index(NULL);
		while (hashtable && ++i < HASH_SIZE)
		{
			mem = (void **)hashtable[i];
			while (mem)
			{
				next = mem[1];
				__real_free(mem[0]);
				__real_free(mem);
				mem = next;
			}
			hashtable[i] = NULL;
		}
		return ;
	}
	mem_delete(ptr, 0);
}
