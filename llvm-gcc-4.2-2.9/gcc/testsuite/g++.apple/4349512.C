/* APPLE LOCAL file 4349512 */
/* { dg-do run } */
/* { dg-options "-O3" } */
extern "C" void abort();
enum VisibilityMode {
	visibilityMode_Hidden = 0,
	visibilityMode_Visible,
	visibilityMode_Closed
};

VisibilityMode GetVisibility();

int x;
void Empty() __attribute__((noinline));
void Empty() { x++; }

void PaletteVisibilityChanged(VisibilityMode inPreviousValue) __attribute__((noinline));
void PaletteVisibilityChanged(VisibilityMode inPreviousValue)
{
	const bool isVisible = (GetVisibility() == visibilityMode_Visible);
	const bool wasHidden = (inPreviousValue != visibilityMode_Visible);
	const bool isHidden = !isVisible;
	
	if (wasHidden == isHidden)
	{
		abort();
	}
	else
	{
		Empty();
	}

	if (wasHidden && isHidden)
	{
		abort();
	}
	else
	{
		Empty();
	}
}

int main (int argc, char * const argv[])
{
	PaletteVisibilityChanged(visibilityMode_Hidden);

    return 0;
}

VisibilityMode GetVisibility()
{
	static VisibilityMode	sValue = visibilityMode_Visible;
	return sValue;
}
