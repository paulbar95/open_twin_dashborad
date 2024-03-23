//! @file WidgetViewManager.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <string>

namespace ads {
	class CDockManager;
	class CDockAreaWidget;
}

namespace ot {

	class WidgetView;
	class WidgetViewCfg;

	class OT_WIDGETS_API_EXPORT WidgetViewManager {
		OT_DECL_NOCOPY(WidgetViewManager)
	public:
		static WidgetViewManager& instance(void);

		//! @brief Must be called upon startup, if no dock manager is provided a new one will be created
		void initialize(ads::CDockManager* _dockManager = nullptr);

		ads::CDockManager* getDockManager(void) const { return m_dockManager; };

		// ###########################################################################################################################################################################################################################################################################################################################

		bool addView(const BasicServiceInformation& _owner, WidgetView* _view);

		bool addView(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _area);
		
		WidgetView* addView(const BasicServiceInformation& _owner, WidgetViewCfg* _viewConfiguration);

		WidgetView* findView(const BasicServiceInformation& _owner, const std::string& _viewName);

		void closeView(const BasicServiceInformation& _owner, const std::string& _viewName);

		void closeViews(const BasicServiceInformation& _owner);

	private:
		WidgetViewManager();
		~WidgetViewManager();

		bool addViewImpl(const BasicServiceInformation& _owner, WidgetView* _view, ads::CDockAreaWidget* _area);

		//! @brief Clear the maps.
		//! Does not free the views!
		void clear(const BasicServiceInformation& _owner);

		ads::CDockManager* m_dockManager;

		std::map<std::string, WidgetView*>* findViewMap(const BasicServiceInformation& _owner);
		std::map<std::string, WidgetView*>* findOrCreateViewMap(const BasicServiceInformation& _owner);

		std::map<BasicServiceInformation, std::map<std::string, WidgetView*>*> m_views;
	};

}