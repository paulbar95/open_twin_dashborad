//! @file BlockPaintJob.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Queue.h"
#include "OTBlockEditor/BlockEditorAPIExport.h"

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace ot {

	class __declspec(dllexport) BlockPaintJobArg : public ot::QueueData {
	public:
		BlockPaintJobArg(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) : m_painter(_painter), m_option(_option), m_widget(_widget) {};
		virtual ~BlockPaintJobArg() {};

		void setPainter(QPainter* _painter) { m_painter = _painter; };
		QPainter* painter(void) const { return m_painter; };
		
		void setStyleOption(const QStyleOptionGraphicsItem* _option) { m_option = _option; };
		const QStyleOptionGraphicsItem* styleOption(void) const { return m_option; };

		void setWidget(QWidget* _widget) { m_widget = _widget; };
		QWidget* widget(void) const { return m_widget; };

	private:
		QPainter* m_painter;
		const QStyleOptionGraphicsItem* m_option;
		QWidget* m_widget;

		BlockPaintJobArg() = delete;
		BlockPaintJobArg(const BlockPaintJobArg&) = delete;
		BlockPaintJobArg& operator = (const BlockPaintJobArg&) = delete;
	};

	class BLOCK_EDITOR_API_EXPORT BlockPaintJob : public ot::QueueObject {
	public:
		BlockPaintJob() {};
		virtual ~BlockPaintJob() {};

		virtual QueueResultFlags activateFromQueue(AbstractQueue* _queue, QueueData* _args) override;
		virtual QueueResultFlags runPaintJob(AbstractQueue* _queue, BlockPaintJobArg* _arg) = 0;

	private:

	};

}