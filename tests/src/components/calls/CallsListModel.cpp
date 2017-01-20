#include <QDebug>
#include <QTimer>

#include "../../app/App.hpp"
#include "../core/CoreManager.hpp"

#include "CallsListModel.hpp"

/* Delay before removing call in ms. */
#define DELAY_BEFORE_REMOVE_CALL 3000

using namespace std;

// =============================================================================

CallsListModel::CallsListModel (QObject *parent) : QAbstractListModel(parent) {
  m_core_handlers = CoreManager::getInstance()->getHandlers();
  QObject::connect(
    &(*m_core_handlers), &CoreHandlers::callStateChanged,
    this, [this](const shared_ptr<linphone::Call> &linphone_call, linphone::CallState state) {
      switch (state) {
        case linphone::CallStateIncomingReceived:
        case linphone::CallStateOutgoingInit:
          addCall(linphone_call);
          break;

        case linphone::CallStateEnd:
        case linphone::CallStateError:
          removeCall(linphone_call);
          break;

        default:
          break;
      }
    }
  );
}

int CallsListModel::rowCount (const QModelIndex &) const {
  return m_list.count();
}

QHash<int, QByteArray> CallsListModel::roleNames () const {
  QHash<int, QByteArray> roles;
  roles[Qt::DisplayRole] = "$call";
  return roles;
}

QVariant CallsListModel::data (const QModelIndex &index, int role) const {
  int row = index.row();

  if (!index.isValid() || row < 0 || row >= m_list.count())
    return QVariant();

  if (role == Qt::DisplayRole)
    return QVariant::fromValue(m_list[row]);

  return QVariant();
}

// -----------------------------------------------------------------------------

bool CallsListModel::removeRow (int row, const QModelIndex &parent) {
  return removeRows(row, 1, parent);
}

bool CallsListModel::removeRows (int row, int count, const QModelIndex &parent) {
  int limit = row + count - 1;

  if (row < 0 || count < 0 || limit >= m_list.count())
    return false;

  beginRemoveRows(parent, row, limit);

  for (int i = 0; i < count; ++i)
    m_list.takeAt(row)->deleteLater();

  endRemoveRows();

  return true;
}

// -----------------------------------------------------------------------------

void CallsListModel::addCall (const shared_ptr<linphone::Call> &linphone_call) {
  App::getInstance()->getCallsWindow()->show();

  CallModel *call = new CallModel(linphone_call);
  App::getInstance()->getEngine()->setObjectOwnership(call, QQmlEngine::CppOwnership);
  linphone_call->setData("call-model", *call);

  int row = m_list.count();

  beginInsertRows(QModelIndex(), row, row);
  m_list << call;
  endInsertRows();
}

void CallsListModel::removeCall (const shared_ptr<linphone::Call> &linphone_call) {
  CallModel *call = &linphone_call->getData<CallModel>("call-model");
  linphone_call->unsetData("call-model");

  // TODO: It will be (maybe) necessary to use a single scheduled function in the future.
  QTimer::singleShot(
    DELAY_BEFORE_REMOVE_CALL, this, [this, call]() {
      qInfo() << "Removing call:" << call;

      int index = m_list.indexOf(call);
      if (index == -1 || !removeRow(index))
        qWarning() << "Unable to remove call:" << call;

      if (m_list.empty())
        App::getInstance()->getCallsWindow()->close();
    }
  );
}