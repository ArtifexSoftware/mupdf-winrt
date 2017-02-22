#pragma once

#include "muctx.h"

int muctx::GetSignature(int page_num, double x, double y, char *contents_ebuff, int *contents_len,
	int *byte_range_ebuff, int *byte_range_len)
{
	int(*byte_range)[2] = NULL;
	fz_point p;
	pdf_ui_event event;
	pdf_document *idoc;
	fz_page *page = NULL;
	pdf_widget *widget = NULL;
	int output = SIG_NOTSIGWIDGET;
	bool have_sizes = true;
	int k;
	char *contents;

	if (contents_ebuff == NULL && byte_range_ebuff == NULL)
		have_sizes = false;

	p.x = (float)x;
	p.y = (float)y;

	event.etype = PDF_EVENT_TYPE_POINTER;
	event.event.pointer.pt = p;
	event.event.pointer.ptype = PDF_POINTER_DOWN;

	fz_var(byte_range);
	fz_var(idoc);
	fz_var(page);
	fz_var(widget);

	fz_try(mu_ctx)
	{
		idoc = pdf_specifics(mu_ctx, mu_doc); /* This just does a casting with null return if not PDF */
		if (idoc != NULL)
		{
			page = LoadPage(page_num);
			if (pdf_pass_event(mu_ctx, idoc, (pdf_page *)page, &event))
				widget = pdf_focused_widget(mu_ctx, idoc);
		}
		if (widget != NULL)
		{
			if (pdf_xref_obj_is_unsaved_signature(idoc, ((pdf_annot *)widget)->obj))
				output = SIG_NOTSAVED;
			else
			{
				*byte_range_len = pdf_signature_widget_byte_range(mu_ctx, idoc, widget, NULL);
				if (*byte_range_len && have_sizes)
				{
					byte_range = (int(*)[2]) fz_calloc(mu_ctx, *byte_range_len, sizeof(*byte_range));
					pdf_signature_widget_byte_range(mu_ctx, idoc, widget, byte_range);
					/* Copy byte range information to an easier object to transfer to c# */
					for (k = 0; k < *byte_range_len; k++)
					{
						byte_range_ebuff[k * 2] = byte_range[k][0];
						byte_range_ebuff[k * 2 + 1] = byte_range[k][1];
					}
				}
				*contents_len = pdf_signature_widget_contents(mu_ctx, idoc, widget, &contents);
				if (*contents_len && have_sizes)
					memcpy(contents_ebuff, contents, *contents_len);
				if (byte_range && contents_ebuff)
					output = SIG_OBTAINED;
				else
					if (have_sizes)
						output = SIG_NOTSIGNED;
					else
						output = SIG_GOTSIZES;
			}
		}
	}
	fz_always(mu_ctx)
	{
		fz_free(mu_ctx, byte_range);
	}
	fz_catch(mu_ctx)
	{
		output = SIG_ERROR;
	}
	return output;
}
