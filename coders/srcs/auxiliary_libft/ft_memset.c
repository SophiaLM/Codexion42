#include "../../includes/codexion.h"

void	*ft_memset(void *dest, int c, size_t count)
{
	char	*destino;
	size_t	i;

	destino = (char *)dest;
	i = 0;
	if (count == 0)
		return (dest);
	while (count--)
		destino[i++] = c;
	return (dest);
}
