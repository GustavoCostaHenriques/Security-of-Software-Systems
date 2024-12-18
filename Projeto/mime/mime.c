
# include "mime.h"
# include <string.h>
#include <assert.h>

void MIME_func(HDR *, ENVELOPE *);
char * HVALUE(char *, HDR *);

int main(int argc, char **argv){
  
  HDR *header;
  register ENVELOPE *e;
  FILE *temp;
 
  assert (argc==2);

  header = (HDR *) malloc(sizeof(struct header));
  
  header->h_field = "Content-Transfer-Encoding";
  header->h_value = "base64";

  e = (ENVELOPE *) malloc(sizeof(struct envelope));
  e->e_id = "First Entry";

  temp = fopen(argv[1],"r");
 
  e->e_dfp = temp;
  MIME_func(header, e);

  fclose(temp);

  return 0;
}

static char index_64[128] =
{
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
	52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1,
	-1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
	15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
	-1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
	41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1
};

#define CHAR64(c)  (((c) < 0 || (c) > 127) ? -1 : index_64[(c)])

/*
**  MIME_func -- output 7 bit encoded MIME body in 8 bit format
**
**  This is a hack. Supports translating the two 7-bit body-encodings
**  (quoted-printable and base64) to 8-bit coded bodies.
**
**  There is not much point in supporting multipart here, as the UA
**  will be able to deal with encoded MIME bodies if it can parse MIME
**  multipart messages.
**
**  Note also that we wont be called unless it is a text/plain MIME
**  message, encoded base64 or QP and mailer flag '9' has been defined
**  on mailer.
**
**	Parameters:
**		header -- the header for this body part.
**		e -- envelope.
**
**	Returns:
**		none.
*/

void
MIME_func(header, e)
	HDR *header;
	register ENVELOPE *e;
{
 
	register char *p;
	u_char *fbufp;
	u_char fbuf[MAXLINE + 1];
	
	p = HVALUE("Content-Transfer-Encoding", header);
	if (p == NULL)
	  {
	    printf("Content-Transfer-Encoding not found in header\n");
	    return;
	  }
   
	/*
	**  Translate body encoding to 8-bit.  Supports two types of
	**  encodings; "base64" and "quoted-printable". Assume qp if
	**  it is not base64.
	*/

	if (strcasecmp(p, "base64") == 0)
	{
		int c1, c2, c3, c4;

		fbufp = fbuf;
		while ((c1 = fgetc(e->e_dfp)) != EOF)
		{
			if (isascii(c1) && isspace(c1))
				continue;

			do
			{
				c2 = fgetc(e->e_dfp);
			} while (isascii(c2) && isspace(c2));
			if (c2 == EOF)
				break;

			do
			{
				c3 = fgetc(e->e_dfp);
			} while (isascii(c3) && isspace(c3));
			if (c3 == EOF)
				break;

			do
			{
				c4 = fgetc(e->e_dfp);
			} while (isascii(c4) && isspace(c4));
			if (c4 == EOF)
				break;

			if (c1 == '=' || c2 == '=')
				continue;
			c1 = CHAR64(c1);
			c2 = CHAR64(c2);
			
			/*We have read in four characters */
			*fbufp = (c1 << 2) | ((c2 & 0x30) >> 4); /* Write first char to fbuf */

			/* Check to see if either 1) reached a '\n' or */
			/* 2) fbufp about to point past end of fbuf */

			if (*fbufp++ == '\n' || fbuf >= &fbuf[MAXLINE]) 
			{
			  if (*--fbufp != '\n' || *--fbufp != '\r') 
			    fbufp++;                               
			                                          
			  *fbufp = '\0';
			  /* putline((char *) fbuf, mci); */
			  printf("resetting fbufp\n");
			  fbufp = fbuf;                          
			                                        
			}

			if (c3 == '=')
				continue;
			c3 = CHAR64(c3);

			*fbufp = ((c2 & 0x0f) << 4) | ((c3 & 0x3c) >> 2);

			if (*fbufp++ == '\n' || fbuf >= &fbuf[MAXLINE])
			{
			
			  if (*--fbufp != '\n' || *--fbufp != '\r')
					fbufp++;
				*fbufp = '\0';
				/* putline((char *) fbuf, mci);*/
				printf("resetting fbufp\n");
				fbufp = fbuf;
			}

			if (c4 == '=')
				continue;
			c4 = CHAR64(c4);

			*fbufp = ((c3 & 0x03) << 6) | c4;

			if (*fbufp++ == '\n' || fbuf >= &fbuf[MAXLINE])
			{
			  if (*--fbufp != '\n' || *--fbufp != '\r')
			    fbufp++;

			  *fbufp = '\0';
			  /* putline((char *) fbuf, mci); */
			  printf("resetting fbufp\n");
			  fbufp = fbuf;
			}
		}

		/* force out partial last line */
		if (fbufp > fbuf)
		{
		  *fbufp = '\0';
		  /* putline((char *) fbuf, mci); */
		}
	}
	else
	{
	  /* do something else...  */
	  printf("We only support base64 encoding...\n");
	}

}



/*
**  HVALUE -- return value of a header.
**
**	Only "real" fields (i.e., ones that have not been supplied
**	as a default) are used.
**
**	Parameters:
**		field -- the field name.
**		header -- the header list.
**
**	Returns:
**		pointer to the value part.
**		NULL if not found.
**
**	Side Effects:
**		none.
*/

char * HVALUE(field, header)
	char *field;
	HDR *header;
{
	register HDR *h;

	for (h = header; h != NULL; h = h->h_link)
	{
		if (!bitset(H_DEFAULT, h->h_flags) &&
		    strcasecmp(h->h_field, field) == 0)
			return (h->h_value);
	}
	return (NULL);
}


