#include "fitz-internal.h"
#include "mupdf-internal.h"

#define SMALL_FLOAT (0.00001)

static pdf_obj *
resolve_dest_rec(pdf_document *xref, pdf_obj *dest, int depth)
{
	if (depth > 10) /* Arbitrary to avoid infinite recursion */
		return NULL;

	if (pdf_is_name(dest) || pdf_is_string(dest))
	{
		dest = pdf_lookup_dest(xref, dest);
		return resolve_dest_rec(xref, dest, depth+1);
	}

	else if (pdf_is_array(dest))
	{
		return dest;
	}

	else if (pdf_is_dict(dest))
	{
		dest = pdf_dict_gets(dest, "D");
		return resolve_dest_rec(xref, dest, depth+1);
	}

	else if (pdf_is_indirect(dest))
		return dest;

	return NULL;
}

static pdf_obj *
resolve_dest(pdf_document *xref, pdf_obj *dest)
{
	return resolve_dest_rec(xref, dest, 0);
}

fz_link_dest
pdf_parse_link_dest(pdf_document *xref, pdf_obj *dest)
{
	fz_link_dest ld;
	pdf_obj *obj;

	int l_from_2 = 0;
	int b_from_3 = 0;
	int r_from_4 = 0;
	int t_from_5 = 0;
	int t_from_3 = 0;
	int t_from_2 = 0;
	int z_from_4 = 0;

	dest = resolve_dest(xref, dest);
	if (dest == NULL || !pdf_is_array(dest))
	{
		ld.kind = FZ_LINK_NONE;
		return ld;
	}
	obj = pdf_array_get(dest, 0);
	if (pdf_is_int(obj))
		ld.ld.gotor.page = pdf_to_int(obj);
	else
		ld.ld.gotor.page = pdf_lookup_page_number(xref, obj);

	ld.kind = FZ_LINK_GOTO;
	ld.ld.gotor.flags = 0;
	ld.ld.gotor.lt.x = 0;
	ld.ld.gotor.lt.y = 0;
	ld.ld.gotor.rb.x = 0;
	ld.ld.gotor.rb.y = 0;
	ld.ld.gotor.file_spec = NULL;
	ld.ld.gotor.new_window = 0;

	obj = pdf_array_get(dest, 1);
	if (!pdf_is_name(obj))
		return ld;

	if (!strcmp("XYZ", pdf_to_name(obj)))
	{
		l_from_2 = t_from_3 = z_from_4 = 1;
		ld.ld.gotor.flags |= fz_link_flag_r_is_zoom;
	}
	else if ((!strcmp("Fit", pdf_to_name(obj))) || (!strcmp("FitB", pdf_to_name(obj))))
	{
		ld.ld.gotor.flags |= fz_link_flag_fit_h;
		ld.ld.gotor.flags |= fz_link_flag_fit_v;
	}
	else if ((!strcmp("FitH", pdf_to_name(obj))) || (!strcmp("FitBH", pdf_to_name(obj))))
	{
		t_from_2 = 1;
		ld.ld.gotor.flags |= fz_link_flag_fit_h;
	}
	else if ((!strcmp("FitV", pdf_to_name(obj))) || (!strcmp("FitBV", pdf_to_name(obj))))
	{
		l_from_2 = 1;
		ld.ld.gotor.flags |= fz_link_flag_fit_v;
	}
	else if (!strcmp("FitR", pdf_to_name(obj)))
	{
		l_from_2 = b_from_3 = r_from_4 = t_from_5 = 1;
		ld.ld.gotor.flags |= fz_link_flag_fit_h;
		ld.ld.gotor.flags |= fz_link_flag_fit_v;
	}

	if (l_from_2)
	{
		obj = pdf_array_get(dest, 2);
		if (pdf_is_int(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_l_valid;
			ld.ld.gotor.lt.x = pdf_to_int(obj);
		}
		else if (pdf_is_real(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_l_valid;
			ld.ld.gotor.lt.x = pdf_to_real(obj);
		}
	}
	if (b_from_3)
	{
		obj = pdf_array_get(dest, 3);
		if (pdf_is_int(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_b_valid;
			ld.ld.gotor.rb.y = pdf_to_int(obj);
		}
		else if (pdf_is_real(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_b_valid;
			ld.ld.gotor.rb.y = pdf_to_real(obj);
		}
	}
	if (r_from_4)
	{
		obj = pdf_array_get(dest, 4);
		if (pdf_is_int(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_r_valid;
			ld.ld.gotor.rb.x = pdf_to_int(obj);
		}
		else if (pdf_is_real(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_r_valid;
			ld.ld.gotor.rb.x = pdf_to_real(obj);
		}
	}
	if (t_from_5 || t_from_3 || t_from_2)
	{
		if (t_from_5)
			obj = pdf_array_get(dest, 5);
		else if (t_from_3)
			obj = pdf_array_get(dest, 3);
		else
			obj = pdf_array_get(dest, 2);
		if (pdf_is_int(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_t_valid;
			ld.ld.gotor.lt.y = pdf_to_int(obj);
		}
		else if (pdf_is_real(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_t_valid;
			ld.ld.gotor.lt.y = pdf_to_real(obj);
		}
	}
	if (z_from_4)
	{
		obj = pdf_array_get(dest, 4);
		if (pdf_is_int(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_r_valid;
			ld.ld.gotor.rb.x = pdf_to_int(obj);
		}
		else if (pdf_is_real(obj))
		{
			ld.ld.gotor.flags |= fz_link_flag_r_valid;
			ld.ld.gotor.rb.x = pdf_to_real(obj);
		}
	}

	/* Duplicate the values out for the sake of stupid clients */
	if ((ld.ld.gotor.flags & (fz_link_flag_l_valid | fz_link_flag_r_valid)) == fz_link_flag_l_valid)
		ld.ld.gotor.rb.x = ld.ld.gotor.lt.x;
	if ((ld.ld.gotor.flags & (fz_link_flag_l_valid | fz_link_flag_r_valid | fz_link_flag_r_is_zoom)) == fz_link_flag_r_valid)
		ld.ld.gotor.lt.x = ld.ld.gotor.rb.x;
	if ((ld.ld.gotor.flags & (fz_link_flag_t_valid | fz_link_flag_b_valid)) == fz_link_flag_t_valid)
		ld.ld.gotor.rb.y = ld.ld.gotor.lt.y;
	if ((ld.ld.gotor.flags & (fz_link_flag_t_valid | fz_link_flag_b_valid)) == fz_link_flag_b_valid)
		ld.ld.gotor.lt.y = ld.ld.gotor.rb.y;

	return ld;
}

static char *
pdf_parse_file_spec(pdf_document *xref, pdf_obj *file_spec)
{
	fz_context *ctx = xref->ctx;
	pdf_obj *filename;

	if (pdf_is_string(file_spec))
		return pdf_to_utf8(xref, file_spec);

	if (pdf_is_dict(file_spec)) {
		filename = pdf_dict_gets(file_spec, "UF");
		if (!filename)
			filename = pdf_dict_gets(file_spec, "F");
		if (!filename)
			filename = pdf_dict_gets(file_spec, "Unix");
		if (!filename)
			filename = pdf_dict_gets(file_spec, "Mac");
		if (!filename)
			filename = pdf_dict_gets(file_spec, "DOS");

		return pdf_to_utf8(xref, filename);
	}

	fz_warn(ctx, "cannot parse file specification");
	return NULL;
}

fz_link_dest
pdf_parse_action(pdf_document *xref, pdf_obj *action)
{
	fz_link_dest ld;
	pdf_obj *obj, *dest;
	fz_context *ctx = xref->ctx;

	UNUSED(ctx);

	ld.kind = FZ_LINK_NONE;

	if (!action)
		return ld;

	obj = pdf_dict_gets(action, "S");
	if (!strcmp(pdf_to_name(obj), "GoTo"))
	{
		dest = pdf_dict_gets(action, "D");
		ld = pdf_parse_link_dest(xref, dest);
	}
	else if (!strcmp(pdf_to_name(obj), "URI"))
	{
		ld.kind = FZ_LINK_URI;
		ld.ld.uri.is_map = pdf_to_bool(pdf_dict_gets(action, "IsMap"));
		ld.ld.uri.uri = pdf_to_utf8(xref, pdf_dict_gets(action, "URI"));
	}
	else if (!strcmp(pdf_to_name(obj), "Launch"))
	{
		ld.kind = FZ_LINK_LAUNCH;
		dest = pdf_dict_gets(action, "F");
		ld.ld.launch.file_spec = pdf_parse_file_spec(xref, dest);
		ld.ld.launch.new_window = pdf_to_int(pdf_dict_gets(action, "NewWindow"));
	}
	else if (!strcmp(pdf_to_name(obj), "Named"))
	{
		ld.kind = FZ_LINK_NAMED;
		ld.ld.named.named = pdf_to_utf8(xref, pdf_dict_gets(action, "N"));
	}
	else if (!strcmp(pdf_to_name(obj), "GoToR"))
	{
		dest = pdf_dict_gets(action, "D");
		ld = pdf_parse_link_dest(xref, dest);
		ld.kind = FZ_LINK_GOTOR;
		dest = pdf_dict_gets(action, "F");
		ld.ld.gotor.file_spec = pdf_parse_file_spec(xref, dest);
		ld.ld.gotor.new_window = pdf_to_int(pdf_dict_gets(action, "NewWindow"));
	}
	return ld;
}

static fz_link *
pdf_load_link(pdf_document *xref, pdf_obj *dict, const fz_matrix *page_ctm)
{
	pdf_obj *dest = NULL;
	pdf_obj *action;
	pdf_obj *obj;
	fz_rect bbox;
	fz_context *ctx = xref->ctx;
	fz_link_dest ld;

	obj = pdf_dict_gets(dict, "Rect");
	if (obj)
		pdf_to_rect(ctx, obj, &bbox);
	else
		bbox = fz_empty_rect;

	fz_transform_rect(&bbox, page_ctm);

	obj = pdf_dict_gets(dict, "Dest");
	if (obj)
	{
		dest = resolve_dest(xref, obj);
		ld = pdf_parse_link_dest(xref, dest);
	}
	else
	{
		action = pdf_dict_gets(dict, "A");
		/* fall back to additional action button's down/up action */
		if (!action)
			action = pdf_dict_getsa(pdf_dict_gets(dict, "AA"), "U", "D");

		ld = pdf_parse_action(xref, action);
	}
	if (ld.kind == FZ_LINK_NONE)
		return NULL;
	return fz_new_link(ctx, &bbox, ld);
}

fz_link *
pdf_load_link_annots(pdf_document *xref, pdf_obj *annots, const fz_matrix *page_ctm)
{
	fz_link *link, *head, *tail;
	pdf_obj *obj;
	int i, n;

	head = tail = NULL;
	link = NULL;

	n = pdf_array_len(annots);
	for (i = 0; i < n; i++)
	{
		fz_try(xref->ctx)
		{
			obj = pdf_array_get(annots, i);
			link = pdf_load_link(xref, obj, page_ctm);
		}
		fz_catch(xref->ctx)
		{
			link = NULL;
		}

		if (link)
		{
			if (!head)
				head = tail = link;
			else
			{
				tail->next = link;
				tail = link;
			}
		}
	}

	return head;
}

void
pdf_free_annot(fz_context *ctx, pdf_annot *annot)
{
	pdf_annot *next;

	while (annot)
	{
		next = annot->next;
		if (annot->ap)
			pdf_drop_xobject(ctx, annot->ap);
		pdf_drop_obj(annot->obj);
		fz_free(ctx, annot);
		annot = next;
	}
}

static void
pdf_transform_annot(pdf_annot *annot)
{
	fz_rect bbox = annot->ap->bbox;
	fz_rect rect = annot->rect;
	float w, h, x, y;

	fz_transform_rect(&bbox, &annot->ap->matrix);
	if (bbox.x1 == bbox.x0)
		w = 0;
	else
		w = (rect.x1 - rect.x0) / (bbox.x1 - bbox.x0);
	if (bbox.y1 == bbox.y0)
		h = 0;
	else
		h = (rect.y1 - rect.y0) / (bbox.y1 - bbox.y0);
	x = rect.x0 - bbox.x0;
	y = rect.y0 - bbox.y0;

	fz_pre_scale(fz_translate(&annot->matrix, x, y), w, h);
}

fz_annot_type pdf_annot_obj_type(pdf_obj *obj)
{
	char *subtype = pdf_to_name(pdf_dict_gets(obj, "Subtype"));
	if (!strcmp(subtype, "Text"))
		return FZ_ANNOT_TEXT;
	else if (!strcmp(subtype, "Link"))
		return FZ_ANNOT_LINK;
	else if (!strcmp(subtype, "FreeText"))
		return FZ_ANNOT_FREETEXT;
	else if (!strcmp(subtype, "Line"))
		return FZ_ANNOT_LINE;
	else if (!strcmp(subtype, "Square"))
		return FZ_ANNOT_SQUARE;
	else if (!strcmp(subtype, "Circle"))
		return FZ_ANNOT_CIRCLE;
	else if (!strcmp(subtype, "Polygon"))
		return FZ_ANNOT_POLYGON;
	else if (!strcmp(subtype, "PolyLine"))
		return FZ_ANNOT_POLYLINE;
	else if (!strcmp(subtype, "Highlight"))
		return FZ_ANNOT_HIGHLIGHT;
	else if (!strcmp(subtype, "Underline"))
		return FZ_ANNOT_UNDERLINE;
	else if (!strcmp(subtype, "Squiggly"))
		return FZ_ANNOT_SQUIGGLY;
	else if (!strcmp(subtype, "StrikeOut"))
		return FZ_ANNOT_STRIKEOUT;
	else if (!strcmp(subtype, "Stamp"))
		return FZ_ANNOT_STAMP;
	else if (!strcmp(subtype, "Caret"))
		return FZ_ANNOT_CARET;
	else if (!strcmp(subtype, "Ink"))
		return FZ_ANNOT_INK;
	else if (!strcmp(subtype, "Popup"))
		return FZ_ANNOT_POPUP;
	else if (!strcmp(subtype, "FileAttachment"))
		return FZ_ANNOT_FILEATTACHMENT;
	else if (!strcmp(subtype, "Sound"))
		return FZ_ANNOT_SOUND;
	else if (!strcmp(subtype, "Movie"))
		return FZ_ANNOT_MOVIE;
	else if (!strcmp(subtype, "Widget"))
		return FZ_ANNOT_WIDGET;
	else if (!strcmp(subtype, "Screen"))
		return FZ_ANNOT_SCREEN;
	else if (!strcmp(subtype, "PrinterMark"))
		return FZ_ANNOT_PRINTERMARK;
	else if (!strcmp(subtype, "TrapNet"))
		return FZ_ANNOT_TRAPNET;
	else if (!strcmp(subtype, "Watermark"))
		return FZ_ANNOT_WATERMARK;
	else if (!strcmp(subtype, "3D"))
		return FZ_ANNOT_3D;
	else
		return -1;
}

static const char *annot_type_str(fz_annot_type type)
{
	switch (type)
	{
	case FZ_ANNOT_TEXT: return "Text";
	case FZ_ANNOT_LINK: return "Link";
	case FZ_ANNOT_FREETEXT: return "FreeText";
	case FZ_ANNOT_LINE: return "Line";
	case FZ_ANNOT_SQUARE: return "Square";
	case FZ_ANNOT_CIRCLE: return "Circle";
	case FZ_ANNOT_POLYGON: return "Polygon";
	case FZ_ANNOT_POLYLINE: return "PolyLine";
	case FZ_ANNOT_HIGHLIGHT: return "Highlight";
	case FZ_ANNOT_UNDERLINE: return "Underline";
	case FZ_ANNOT_SQUIGGLY: return "Squiggly";
	case FZ_ANNOT_STRIKEOUT: return "StrikeOut";
	case FZ_ANNOT_STAMP: return "Stamp";
	case FZ_ANNOT_CARET: return "Caret";
	case FZ_ANNOT_INK: return "Ink";
	case FZ_ANNOT_POPUP: return "Popup";
	case FZ_ANNOT_FILEATTACHMENT: return "FileAttachment";
	case FZ_ANNOT_SOUND: return "Sound";
	case FZ_ANNOT_MOVIE: return "Movie";
	case FZ_ANNOT_WIDGET: return "Widget";
	case FZ_ANNOT_SCREEN: return "Screen";
	case FZ_ANNOT_PRINTERMARK: return "PrinterMark";
	case FZ_ANNOT_TRAPNET: return "TrapNet";
	case FZ_ANNOT_WATERMARK: return "Watermark";
	case FZ_ANNOT_3D: return "3D";
	default: return "";
	}
}

pdf_annot *
pdf_load_annots(pdf_document *xref, pdf_obj *annots, pdf_page *page)
{
	pdf_annot *annot, *head, *tail;
	pdf_obj *obj, *ap, *as, *n, *rect;
	int i, len, is_dict;
	fz_context *ctx = xref->ctx;

	fz_var(annot);

	head = tail = NULL;

	len = pdf_array_len(annots);
	for (i = 0; i < len; i++)
	{
		fz_try(ctx)
		{
			obj = pdf_array_get(annots, i);

			if (xref->update_appearance)
				xref->update_appearance(xref, obj);

			rect = pdf_dict_gets(obj, "Rect");
			ap = pdf_dict_gets(obj, "AP");
			as = pdf_dict_gets(obj, "AS");
			is_dict = pdf_is_dict(ap);
		}
		fz_catch(ctx)
		{
			ap = NULL;
			is_dict = 0;
		}

		if (!is_dict)
			continue;

		annot = NULL;
		fz_try(ctx)
		{
			pdf_hotspot *hp = &xref->hotspot;

			n = NULL;

			if (hp->num == pdf_to_num(obj)
				&& hp->gen == pdf_to_gen(obj)
				&& (hp->state & HOTSPOT_POINTER_DOWN))
			{
				n = pdf_dict_gets(ap, "D"); /* down state */
			}

			if (n == NULL)
				n = pdf_dict_gets(ap, "N"); /* normal state */

			/* lookup current state in sub-dictionary */
			if (!pdf_is_stream(xref, pdf_to_num(n), pdf_to_gen(n)))
				n = pdf_dict_get(n, as);

			annot = fz_malloc_struct(ctx, pdf_annot);
			annot->page = page;
			annot->obj = pdf_keep_obj(obj);
			pdf_to_rect(ctx, rect, &annot->rect);
			annot->pagerect = annot->rect;
			fz_transform_rect(&annot->pagerect, &page->ctm);
			annot->ap = NULL;
			annot->annot_type = pdf_annot_obj_type(obj);
			annot->widget_type = annot->annot_type == FZ_ANNOT_WIDGET ? pdf_field_type(xref, obj) : FZ_WIDGET_TYPE_NOT_WIDGET;

			if (pdf_is_stream(xref, pdf_to_num(n), pdf_to_gen(n)))
			{
				annot->ap = pdf_load_xobject(xref, n);
				pdf_transform_annot(annot);
				annot->ap_iteration = annot->ap->iteration;
			}

			annot->next = NULL;

			if (obj == xref->focus_obj)
				xref->focus = annot;

			if (!head)
				head = tail = annot;
			else
			{
				tail->next = annot;
				tail = annot;
			}
		}
		fz_catch(ctx)
		{
			pdf_free_annot(ctx, annot);
			fz_warn(ctx, "ignoring broken annotation");
		}
	}

	return head;
}

void
pdf_update_annot(pdf_document *xref, pdf_annot *annot)
{
	pdf_obj *obj, *ap, *as, *n;
	fz_context *ctx = xref->ctx;

	obj = annot->obj;

	if (xref->update_appearance)
		xref->update_appearance(xref, obj);

	ap = pdf_dict_gets(obj, "AP");
	as = pdf_dict_gets(obj, "AS");

	if (pdf_is_dict(ap))
	{
		pdf_hotspot *hp = &xref->hotspot;

		n = NULL;

		if (hp->num == pdf_to_num(obj)
			&& hp->gen == pdf_to_gen(obj)
			&& (hp->state & HOTSPOT_POINTER_DOWN))
		{
			n = pdf_dict_gets(ap, "D"); /* down state */
		}

		if (n == NULL)
			n = pdf_dict_gets(ap, "N"); /* normal state */

		/* lookup current state in sub-dictionary */
		if (!pdf_is_stream(xref, pdf_to_num(n), pdf_to_gen(n)))
			n = pdf_dict_get(n, as);

		pdf_drop_xobject(ctx, annot->ap);
		annot->ap = NULL;

		if (pdf_is_stream(xref, pdf_to_num(n), pdf_to_gen(n)))
		{
			fz_try(ctx)
			{
				annot->ap = pdf_load_xobject(xref, n);
				pdf_transform_annot(annot);
				annot->ap_iteration = annot->ap->iteration;
			}
			fz_catch(ctx)
			{
				fz_warn(ctx, "ignoring broken annotation");
			}
		}
	}
}

pdf_annot *
pdf_first_annot(pdf_document *doc, pdf_page *page)
{
	return page ? page->annots : NULL;
}

pdf_annot *
pdf_next_annot(pdf_document *doc, pdf_annot *annot)
{
	return annot ? annot->next : NULL;
}

fz_rect *
pdf_bound_annot(pdf_document *doc, pdf_annot *annot, fz_rect *rect)
{
	if (rect == NULL)
		return NULL;

	if (annot)
		*rect = annot->pagerect;
	else
		*rect = fz_empty_rect;
	return rect;
}

fz_annot_type
pdf_annot_type(pdf_annot *annot)
{
	return annot->annot_type;
}

pdf_annot *
pdf_create_annot(pdf_document *doc, pdf_page *page, fz_annot_type type)
{
	fz_context *ctx = doc->ctx;
	pdf_annot *annot = NULL;
	pdf_obj *annot_obj = pdf_new_dict(ctx, 0);
	pdf_obj *ind_obj = NULL;

	fz_var(annot);
	fz_var(ind_obj);
	fz_try(ctx)
	{
		int ind_obj_num;
		fz_rect rect = {0.0, 0.0, 0.0, 0.0};
		const char *type_str = annot_type_str(type);
		pdf_obj *annot_arr = pdf_dict_gets(page->me, "Annots");
		if (annot_arr == NULL)
		{
			annot_arr = pdf_new_array(ctx, 0);
			pdf_dict_puts_drop(page->me, "Annots", annot_arr);
		}

		pdf_dict_puts_drop(annot_obj, "Type", pdf_new_name(ctx, "Annot"));

		pdf_dict_puts_drop(annot_obj, "Subtype", pdf_new_name(ctx, type_str));
		pdf_dict_puts_drop(annot_obj, "Rect", pdf_new_rect(ctx, &rect));

		annot = fz_malloc_struct(ctx, pdf_annot);
		annot->page = page;
		annot->obj = pdf_keep_obj(annot_obj);
		annot->rect = rect;
		annot->pagerect = rect;
		annot->ap = NULL;
		annot->widget_type = FZ_WIDGET_TYPE_NOT_WIDGET;
		annot->annot_type = type;

		/*
			Both annotation object and annotation structure are now created.
			Insert the object in the hierarchy and the structure in the
			page's array.
		*/
		ind_obj_num = pdf_create_object(doc);
		pdf_update_object(doc, ind_obj_num, annot_obj);
		ind_obj = pdf_new_indirect(ctx, ind_obj_num, 0, doc);
		pdf_array_push(annot_arr, ind_obj);

		/*
			Linking must be done before any call that might throw because
			pdf_free_annot below actually frees a list
		*/
		annot->next = page->annots;
		page->annots = annot;

		doc->dirty = 1;
	}
	fz_always(ctx)
	{
		pdf_drop_obj(annot_obj);
		pdf_drop_obj(ind_obj);
	}
	fz_catch(ctx)
	{
		pdf_free_annot(ctx, annot);
		fz_rethrow(ctx);
	}

	return annot;
}

void
pdf_delete_annot(pdf_document *doc, pdf_page *page, pdf_annot *annot)
{
	fz_context *ctx = doc->ctx;
	pdf_annot **annotptr;
	pdf_obj *old_annot_arr;
	pdf_obj *annot_arr;

	if (annot == NULL)
		return;

	/* Remove annot from page's list */
	for (annotptr = &page->annots; *annotptr; annotptr = &(*annotptr)->next)
	{
		if (*annotptr == annot)
			break;
	}

	/* Check the passed annotation was of this page */
	if (*annotptr == NULL)
		return;

	*annotptr = annot->next;

	/* Stick it in the deleted list */
	annot->next = page->deleted_annots;
	page->deleted_annots = annot;

	pdf_drop_xobject(ctx, annot->ap);
	annot->ap = NULL;

	/* Recreate the "Annots" array with this annot removed */
	old_annot_arr = pdf_dict_gets(page->me, "Annots");

	if (old_annot_arr)
	{
		int i, n = pdf_array_len(old_annot_arr);
		annot_arr = pdf_new_array(ctx, n?(n-1):0);

		fz_try(ctx)
		{
			for (i = 0; i < n; i++)
			{
				pdf_obj *obj = pdf_array_get(old_annot_arr, i);

				if (obj != annot->obj)
					pdf_array_push(annot_arr, obj);
			}

			/*
			Overwrite "Annots" in the page dictionary, which has the
			side-effect of releasing the last reference to old_annot_arr
			*/
			pdf_dict_puts(page->me, "Annots", annot_arr);
		}
		fz_always(ctx)
		{
			pdf_drop_obj(annot_arr);
		}
		fz_catch(ctx)
		{
			fz_rethrow(ctx);
		}
	}

	pdf_drop_obj(annot->obj);
	annot->obj = NULL;
	doc->dirty = 1;
}

static fz_colorspace *pdf_to_color(pdf_document *doc, pdf_obj *col, float color[4])
{
	fz_colorspace *cs;
	int i, ncol = pdf_array_len(col);

	switch (ncol)
	{
	case 1: cs = fz_device_gray(doc->ctx); break;
	case 3: cs = fz_device_rgb(doc->ctx); break;
	case 4: cs = fz_device_cmyk(doc->ctx); break;
	default: return NULL;
	}

	for (i = 0; i < ncol; i++)
		color[i] = pdf_to_real(pdf_array_get(col, i));

	return cs;
}

static fz_point *
quadpoints(pdf_document *doc, pdf_obj *annot, int *nout)
{
	fz_context *ctx = doc->ctx;
	pdf_obj *quad = pdf_dict_gets(annot, "QuadPoints");
	fz_point *qp = NULL;
	int i, n;

	if (!quad)
		return NULL;

	n = pdf_array_len(quad);

	if (n%8 != 0)
		return NULL;

	fz_var(qp);
	fz_try(ctx)
	{
		qp = fz_malloc_array(ctx, n/2, sizeof(fz_point));

		for (i = 0; i < n; i += 2)
		{
			qp[i/2].x = pdf_to_real(pdf_array_get(quad, i));
			qp[i/2].y = pdf_to_real(pdf_array_get(quad, i+1));
		}
	}
	fz_catch(ctx)
	{
		fz_free(ctx, qp);
		fz_rethrow(ctx);
	}

	*nout = n/2;

	return qp;
}

void
pdf_set_markup_annot_quadpoints(pdf_document *doc, pdf_annot *annot, fz_point *qp, int n)
{
	fz_context *ctx = doc->ctx;
	fz_matrix ctm;
	pdf_obj *arr = pdf_new_array(ctx, n*2);
	int i;

	fz_invert_matrix(&ctm, &annot->page->ctm);

	pdf_dict_puts_drop(annot->obj, "QuadPoints", arr);

	for (i = 0; i < n; i++)
	{
		fz_point pt = qp[i];
		pdf_obj *r;

		fz_transform_point(&pt, &ctm);
		r = pdf_new_real(ctx, pt.x);
		pdf_array_push_drop(arr, r);
		r = pdf_new_real(ctx, pt.y);
		pdf_array_push_drop(arr, r);
	}
}

static void update_rect(fz_context *ctx, pdf_annot *annot)
{
	pdf_to_rect(ctx, pdf_dict_gets(annot->obj, "Rect"), &annot->rect);
	annot->pagerect = annot->rect;
	fz_transform_rect(&annot->pagerect, &annot->page->ctm);
}

void
pdf_set_ink_annot_list(pdf_document *doc, pdf_annot *annot, fz_point *pts, int *counts, int ncount, float color[3], float thickness)
{
	fz_context *ctx = doc->ctx;
	fz_matrix ctm;
	pdf_obj *list = pdf_new_array(ctx, ncount);
	pdf_obj *bs, *col;
	fz_rect rect;
	int i, k = 0;

	fz_invert_matrix(&ctm, &annot->page->ctm);

	pdf_dict_puts_drop(annot->obj, "InkList", list);

	for (i = 0; i < ncount; i++)
	{
		int j;
		pdf_obj *arc = pdf_new_array(ctx, counts[i]);

		pdf_array_push_drop(list, arc);

		for (j = 0; j < counts[i]; j++)
		{
			fz_point pt = pts[k];

			fz_transform_point(&pt, &ctm);

			if (i == 0 && j == 0)
			{
				rect.x0 = rect.x1 = pt.x;
				rect.y0 = rect.y1 = pt.y;
			}
			else
			{
				fz_include_point_in_rect(&rect, &pt);
			}

			pdf_array_push_drop(arc, pdf_new_real(ctx, pt.x));
			pdf_array_push_drop(arc, pdf_new_real(ctx, pt.y));
			k++;
		}
	}

	fz_expand_rect(&rect, thickness);
	pdf_dict_puts_drop(annot->obj, "Rect", pdf_new_rect(ctx, &rect));
	update_rect(ctx, annot);

	bs = pdf_new_dict(ctx, 1);
	pdf_dict_puts_drop(annot->obj, "BS", bs);
	pdf_dict_puts_drop(bs, "W", pdf_new_real(ctx, thickness));

	col = pdf_new_array(ctx, 3);
	pdf_dict_puts_drop(annot->obj, "C", col);
	for (i = 0; i < 3; i++)
		pdf_array_push_drop(col, pdf_new_real(ctx, color[i]));
}

void
pdf_set_annot_obj_appearance(pdf_document *doc, pdf_obj *obj, const fz_matrix *page_ctm, fz_rect *rect, fz_display_list *disp_list)
{
	fz_context *ctx = doc->ctx;
	fz_matrix ctm;
	fz_matrix mat = fz_identity;
	fz_device *dev = NULL;
	pdf_xobject *xobj = NULL;

	fz_invert_matrix(&ctm, page_ctm);

	fz_var(dev);
	fz_try(ctx)
	{
		pdf_obj *ap_obj;
		fz_rect trect = *rect;

		fz_transform_rect(&trect, &ctm);

		pdf_dict_puts_drop(obj, "Rect", pdf_new_rect(ctx, &trect));

		/* See if there is a current normal appearance */
		ap_obj = pdf_dict_getp(obj, "AP/N");
		if (!pdf_is_stream(doc, pdf_to_num(obj), pdf_to_gen(obj)))
			ap_obj = NULL;

		if (ap_obj == NULL)
		{
			ap_obj = pdf_new_xobject(doc, &trect, &mat);
			pdf_dict_putp_drop(obj, "AP/N", ap_obj);
		}
		else
		{
			pdf_dict_puts_drop(ap_obj, "Rect", pdf_new_rect(ctx, &trect));
			pdf_dict_puts_drop(ap_obj, "Matrix", pdf_new_matrix(ctx, &mat));
		}

		dev = pdf_new_pdf_device(doc, ap_obj, pdf_dict_gets(ap_obj, "Resources"), &mat);
		fz_run_display_list(disp_list, dev, &ctm, &fz_infinite_rect, NULL);
		fz_free_device(dev);

		/* Mark the appearance as changed - required for partial update */
		xobj = pdf_load_xobject(doc, ap_obj);
		if (xobj)
		{
			xobj->iteration++;
			pdf_drop_xobject(ctx, xobj);
		}

		doc->dirty = 1;
	}
	fz_catch(ctx)
	{
		fz_free_device(dev);
		fz_rethrow(ctx);
	}
}

void
pdf_set_annot_appearance(pdf_document *doc, pdf_annot *annot, fz_rect *rect, fz_display_list *disp_list)
{
	pdf_set_annot_obj_appearance(doc, annot->obj, &annot->page->ctm, rect, disp_list);
	update_rect(doc->ctx, annot);
}

void
pdf_set_markup_obj_appearance(pdf_document *doc, pdf_obj *annot, float color[3], float alpha, float line_thickness, float line_height)
{
	fz_context *ctx = doc->ctx;
	fz_path *path = NULL;
	fz_stroke_state *stroke = NULL;
	fz_device *dev = NULL;
	fz_display_list *strike_list = NULL;
	int i, n;
	fz_point *qp = quadpoints(doc, annot, &n);

	if (!qp || n <= 0)
		return;

	fz_var(path);
	fz_var(stroke);
	fz_var(dev);
	fz_var(strike_list);
	fz_try(ctx)
	{
		fz_rect rect = fz_empty_rect;

		rect.x0 = rect.x1 = qp[0].x;
		rect.y0 = rect.y1 = qp[0].y;
		for (i = 0; i < n; i++)
			fz_include_point_in_rect(&rect, &qp[i]);

		strike_list = fz_new_display_list(ctx);
		dev = fz_new_list_device(ctx, strike_list);

		for (i = 0; i < n; i += 4)
		{
			fz_point pt0 = qp[i];
			fz_point pt1 = qp[i+1];
			fz_point up;
			float thickness;

			up.x = qp[i+2].x - qp[i+1].x;
			up.y = qp[i+2].y - qp[i+1].y;

			pt0.x += line_height * up.x;
			pt0.y += line_height * up.y;
			pt1.x += line_height * up.x;
			pt1.y += line_height * up.y;

			thickness = sqrtf(up.x * up.x + up.y * up.y) * line_thickness;

			if (!stroke || fz_abs(stroke->linewidth - thickness) < SMALL_FLOAT)
			{
				if (stroke)
				{
					// assert(path)
					fz_stroke_path(dev, path, stroke, &fz_identity, fz_device_rgb(ctx), color, alpha);
					fz_drop_stroke_state(ctx, stroke);
					stroke = NULL;
					fz_free_path(ctx, path);
					path = NULL;
				}

				stroke = fz_new_stroke_state(ctx);
				stroke->linewidth = thickness;
				path = fz_new_path(ctx);
			}

			fz_moveto(ctx, path, pt0.x, pt0.y);
			fz_lineto(ctx, path, pt1.x, pt1.y);
		}

		if (stroke)
		{
			fz_stroke_path(dev, path, stroke, &fz_identity, fz_device_rgb(ctx), color, alpha);
		}

		pdf_set_annot_obj_appearance(doc, annot, &fz_identity, &rect, strike_list);
	}
	fz_always(ctx)
	{
		fz_free(ctx, qp);
		fz_free_device(dev);
		fz_drop_stroke_state(ctx, stroke);
		fz_free_path(ctx, path);
		fz_free_display_list(ctx, strike_list);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void
pdf_set_ink_obj_appearance(pdf_document *doc, pdf_obj *annot)
{
	fz_context *ctx = doc->ctx;
	fz_path *path = NULL;
	fz_stroke_state *stroke = NULL;
	fz_device *dev = NULL;
	fz_display_list *strike_list = NULL;

	fz_var(path);
	fz_var(stroke);
	fz_var(dev);
	fz_var(strike_list);
	fz_try(ctx)
	{
		fz_rect rect = fz_empty_rect;
		fz_colorspace *cs;
		float color[4];
		float width;
		pdf_obj *list;
		int n, m, i, j;

		cs = pdf_to_color(doc, pdf_dict_gets(annot, "C"), color);
		if (!cs)
		{
			cs = fz_device_rgb(ctx);
			color[0] = 1.0f;
			color[1] = 0.0f;
			color[2] = 0.0f;
		}

		width = pdf_to_real(pdf_dict_gets(pdf_dict_gets(annot, "BS"), "W"));
		if (width == 0.0f)
			width = 1.0f;

		list = pdf_dict_gets(annot, "InkList");

		n = pdf_array_len(list);

		strike_list = fz_new_display_list(ctx);
		dev = fz_new_list_device(ctx, strike_list);
		path = fz_new_path(ctx);
		stroke = fz_new_stroke_state(ctx);
		stroke->linewidth = width;

		for (i = 0; i < n; i ++)
		{
			pdf_obj *arc = pdf_array_get(list, i);
			m = pdf_array_len(arc);

			for (j = 0; j < m-1; j += 2)
			{
				fz_point pt;
				pt.x = pdf_to_real(pdf_array_get(arc, j));
				pt.y = pdf_to_real(pdf_array_get(arc, j+1));

				if (i == 0 && j == 0)
				{
					rect.x0 = rect.x1 = pt.x;
					rect.y0 = rect.y1 = pt.y;
				}
				else
				{
					fz_include_point_in_rect(&rect, &pt);
				}

				if (j == 0)
					fz_moveto(ctx, path, pt.x, pt.y);
				else
					fz_lineto(ctx, path, pt.x, pt.y);
			}
		}

		fz_stroke_path(dev, path, stroke, &fz_identity, cs, color, 1.0f);

		fz_expand_rect(&rect, width);

		pdf_set_annot_obj_appearance(doc, annot, &fz_identity, &rect, strike_list);
	}
	fz_always(ctx)
	{
		fz_free_device(dev);
		fz_drop_stroke_state(ctx, stroke);
		fz_free_path(ctx, path);
		fz_free_display_list(ctx, strike_list);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}
}

void
pdf_set_markup_appearance(pdf_document *doc, pdf_annot *annot, float color[3], float alpha, float line_thickness, float line_height)
{
	pdf_set_markup_obj_appearance(doc, annot->obj, color, alpha, line_thickness, line_height);
	update_rect(doc->ctx, annot);
}
