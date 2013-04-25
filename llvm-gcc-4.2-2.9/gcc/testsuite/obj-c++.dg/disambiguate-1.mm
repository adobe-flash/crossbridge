/* APPLE LOCAL file radar 5355344 */
/* Test that parser can disambiguate a conditional expression from a protocol type use. */

/* { dg-do run } */
int m_nMinID, m_nMaxID;
extern "C" void abort (void);

int MyFunction(int id)
{  
    return (id < m_nMinID || id > m_nMaxID) ;
}

int main()
{
	if (!MyFunction (100))
	  abort ();
	return 0;
}

