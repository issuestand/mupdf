#include "fitz.h"
#include "mupdf-internal.h"

/*
	PDF is currently the only interactive format, so no need
	to indirect through function pointers.
*/

int fz_has_unsaved_changes(fz_interactive *idoc)
{
	return pdf_has_unsaved_changes((pdf_document*)idoc);
}

int fz_pass_event(fz_interactive *idoc, fz_page *page, fz_ui_event *ui_event)
{
	return pdf_pass_event((pdf_document*)idoc, (pdf_page*)page, ui_event);
}

void fz_update_page(fz_interactive *idoc, fz_page *page)
{
	pdf_update_page((pdf_document*)idoc, (pdf_page*)page);
}

fz_annot *fz_poll_changed_annot(fz_interactive *idoc, fz_page *page)
{
	return (fz_annot*)pdf_poll_changed_annot((pdf_document*)idoc, (pdf_page*)page);
}

fz_widget *fz_focused_widget(fz_interactive *idoc)
{
	return pdf_focused_widget((pdf_document*)idoc);
}

fz_widget *fz_first_widget(fz_interactive *idoc, fz_page *page)
{
	return pdf_first_widget((pdf_document*)idoc, (pdf_page*)page);
}

fz_widget *fz_next_widget(fz_interactive *idoc, fz_widget *previous)
{
	return pdf_next_widget(previous);
}

char *fz_text_widget_text(fz_interactive *idoc, fz_widget *tw)
{
	return pdf_text_widget_text((pdf_document *)idoc, tw);
}

int fz_text_widget_max_len(fz_interactive *idoc, fz_widget *tw)
{
	return pdf_text_widget_max_len((pdf_document *)idoc, tw);
}

int fz_text_widget_content_type(fz_interactive *idoc, fz_widget *tw)
{
	return pdf_text_widget_content_type((pdf_document *)idoc, tw);
}

int fz_text_widget_set_text(fz_interactive *idoc, fz_widget *tw, char *text)
{
	return pdf_text_widget_set_text((pdf_document *)idoc, tw, text);
}

int fz_choice_widget_options(fz_interactive *idoc, fz_widget *tw, char *opts[])
{
	return pdf_choice_widget_options((pdf_document *)idoc, tw, opts);
}

int fz_choice_widget_is_multiselect(fz_interactive *idoc, fz_widget *tw)
{
	return pdf_choice_widget_is_multiselect((pdf_document *)idoc, tw);
}

int fz_choice_widget_value(fz_interactive *idoc, fz_widget *tw, char *opts[])
{
	return pdf_choice_widget_value((pdf_document *)idoc, tw, opts);
}

void fz_choice_widget_set_value(fz_interactive *idoc, fz_widget *tw, int n, char *opts[])
{
	pdf_choice_widget_set_value((pdf_document *)idoc, tw, n, opts);
}

int fz_signature_widget_byte_range(fz_interactive *idoc, fz_widget *widget, int (*byte_range)[2])
{
	return pdf_signature_widget_byte_range((pdf_document *)idoc, widget, byte_range);
}

int fz_signature_widget_contents(fz_interactive *idoc, fz_widget *widget, char **contents)
{
	return pdf_signature_widget_contents((pdf_document *)idoc, widget, contents);
}

fz_annot_type fz_get_annot_type(fz_annot *annot)
{
	return pdf_annot_type((pdf_annot *)annot);
}

fz_annot *fz_create_annot(fz_interactive *idoc, fz_page *page, fz_annot_type type)
{
	return (fz_annot *)pdf_create_annot((pdf_document *)idoc, (pdf_page *)page, type);
}

void fz_delete_annot(fz_interactive *idoc, fz_page *page, fz_annot *annot)
{
	pdf_delete_annot((pdf_document *)idoc, (pdf_page *)page, (pdf_annot *)annot);
}

void fz_set_annot_appearance(fz_interactive *idoc, fz_annot *annot, fz_rect *rect, fz_display_list *disp_list)
{
	pdf_set_annot_appearance((pdf_document *)idoc, (pdf_annot *)annot, rect, disp_list);
}

void fz_set_markup_annot_quadpoints(fz_interactive *idoc, fz_annot *annot, fz_point *qp, int n)
{
	pdf_set_markup_annot_quadpoints((pdf_document *)idoc, (pdf_annot *)annot, qp, n);
}

void fz_set_markup_appearance(fz_interactive *idoc, fz_annot *annot, float color[3], float alpha, float line_thickness, float line_height)
{
	pdf_set_markup_appearance((pdf_document *)idoc, (pdf_annot *)annot, color, alpha, line_thickness, line_height);
}

void fz_set_ink_annot_list(fz_interactive *idoc, fz_annot *annot, fz_point *pts, int *counts, int ncount, float color[3], float thickness)
{
	pdf_set_ink_annot_list((pdf_document *)idoc, (pdf_annot *)annot, pts, counts, ncount, color, thickness);
}

void fz_set_doc_event_callback(fz_interactive *idoc, fz_doc_event_cb *event_cb, void *data)
{
	pdf_set_doc_event_callback((pdf_document *)idoc, event_cb, data);
}
