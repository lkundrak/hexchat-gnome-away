/*
 * GNOME ScreenSaver auto-away Hexchat plugin
 * Copyright (C) 2017  Lubomir Rintel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <hexchat-plugin.h>
#include <gio/gio.h>

static void
set_away (hexchat_plugin *plugin_handle, GVariant *screensaver_state)
{
	gboolean active;

	g_variant_get (screensaver_state, "(b)", &active);
	hexchat_command (plugin_handle, active ? "AWAY Away" : "BACK");
}

static void
active_changed (GDBusConnection *connection,
                const gchar *sender_name,
                const gchar *object_path,
                const gchar *interface_name,
                const gchar *signal_name,
                GVariant *parameters,
                gpointer user_data)
{
	set_away ((hexchat_plugin *)user_data, parameters);
}

int
hexchat_plugin_init (hexchat_plugin *plugin_handle,
                     char **plugin_name, char **plugin_desc,
                     char **plugin_version, char *arg)
{
	GDBusProxy *proxy;
	GError *error = NULL;
	GVariant *res;

	*plugin_name = "hexchat-gnome-away";
	*plugin_desc = "GNOME ScreenSaver auto-away";
	*plugin_version = "0.1";

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       "org.gnome.ScreenSaver",
	                                       "/org/gnome/ScreenSaver",
	                                       "org.gnome.ScreenSaver",
	                                       NULL,
	                                       &error);
	if (proxy == NULL) {
		hexchat_printf (plugin_handle,
		                "Could not subscribe to the screensaver: %s\n",
		                error->message);
		g_error_free (error);
	        return 0;
	}

	res = g_dbus_proxy_call_sync (proxy,
	                              "GetActive",
	                              NULL,
	                              G_DBUS_CALL_FLAGS_NONE,
	                              -1,
	                              NULL,
	                              &error);
	if (res == NULL) {
		hexchat_printf (plugin_handle,
		                "Could not query screensaver state: %s\n",
		                error->message);
		g_error_free (error);
	} else {
		set_away (plugin_handle, res);
		g_variant_unref (res);
	}

	g_dbus_connection_signal_subscribe (g_dbus_proxy_get_connection (proxy),
	                                    NULL,
	                                    "org.gnome.ScreenSaver",
	                                    "ActiveChanged",
	                                    "/org/gnome/ScreenSaver",
	                                    NULL,
	                                    G_DBUS_SIGNAL_FLAGS_NONE,
	                                    active_changed,
	                                    plugin_handle,
	                                    NULL);

	return 1;
}
