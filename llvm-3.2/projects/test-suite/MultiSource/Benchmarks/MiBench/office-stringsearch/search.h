/* +++Date last modified: 05-Jul-1997 */

/*
**  SNIPPETS string searching functions
*/

void  init_search(const char *string);                /* Pbmsrch.C      */
char *strsearch(const char *string);                  /* Pbmsrch.C      */
void  bmh_init(const char *pattern);                  /* Bmhsrch.C      */
char *bmh_search(const char *string,                  /* Bmhsrch.C      */
                 const int stringlen);
void  bmhi_init(const char *pattern);                 /* Bhmisrch.C     */
char *bmhi_search(const char *string,                 /* Bhmisrch.C     */
                  const int stringlen);
void  bmha_init(const char *pattern);                 /* Bmhasrch.C     */
char *bmha_search(const char *string,                 /* Bmhasrch.C     */
                  const int stringlen);
